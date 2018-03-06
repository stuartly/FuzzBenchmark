-- QUERY START:
if not exists (select * from sysobjects where name='tig_socks5_users' and xtype='U')
	CREATE  TABLE [dbo].[tig_socks5_users] (
	-- unique uid for fast lookup
	[uid] [bigint] IDENTITY(1,1) NOT NULL,
	-- jid of user
	[user_id] nvarchar(2049) NOT NULL,
	-- sha1 hash of user_id for fast lookup
	[sha1_user_id] [varbinary](128) NOT NULL,

	-- domain part of jid of user
	[domain] nvarchar(2049) NOT NULL,
	[domain_fragment] AS LEFT ([domain], 765), 
	-- sha1 hash of domain part of user_id for fast lookup
	[sha1_domain] [varbinary](128) NOT NULL,

	-- limit of file size 
	[filesize_limit] [bigint] default 0,
	-- limit of transfer per user (0 - get default, -1 - deny any transfer)
	[transfer_limit_per_user] [bigint] default 0,
	-- limit of transfer per domain (0 - get default, -1 - deny any transfer)
	[transfer_limit_per_domain] [bigint] default 0,
	-- helper column
	user_id_fragment AS LEFT (user_id, 765), 

	primary key (uid),
	CONSTRAINT [IX_tig_socks5_users_sha1_user_id] UNIQUE NONCLUSTERED ( [sha1_user_id] ASC ) ON [PRIMARY]
	);
-- QUERY END:
GO

-- QUERY START:
create unique index tig_socks5_users_user_id on dbo.tig_socks5_users ( user_id_fragment );
-- QUERY END:
GO

-- QUERY START:
create unique index tig_socks5_users_domain on dbo.tig_socks5_users ( [domain_fragment] );
-- QUERY END:
GO

-- QUERY START:
if not exists (select * from sysobjects where name='tig_socks5_connections' and xtype='U')
	create table [dbo].[tig_socks5_connections] (

	-- id of connection for fast lookup for update
	[conn_id] [bigint] IDENTITY(1,1),

	-- uid of user (uid of jid)
	[uid] [bigint] NOT NULL,

    -- server instance used as proxy
    [instance] nvarchar(128) NOT NULL,

	-- direction of transfer,  -- 0-in, 1-out
	[direction] int NOT NULL,

	-- count of bytes transferred thru connections
	[transferred_bytes] bigint default 0,

	-- timestamp of last part transfer
	[transfer_timestamp] [datetime] DEFAULT CURRENT_TIMESTAMP,

	primary key (conn_id),

)
-- QUERY END:
GO

-- QUERY START:
create index tig_socks5_connections_uid on dbo.tig_socks5_connections ( uid );
-- QUERY END:
GO

-- QUERY START:
create index tig_socks5_connections_uid_transfer_timestamp on dbo.tig_socks5_connections ( uid, transfer_timestamp );
-- QUERY END:
GO

-- QUERY START:
create index tig_socks5_connections_instance_transfer_timestamp on dbo.tig_socks5_connections ( instance, transfer_timestamp );
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5CreateUid')
DROP PROCEDURE TigSocks5CreateUid
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigSocks5CreateUid
	@_user_id nvarchar(2049),
	@_domain nvarchar(2049)
AS	
begin	
	insert into dbo.tig_socks5_users ([user_id], [sha1_user_id], [domain], [sha1_domain]) 
		values (@_user_id, HASHBYTES('SHA1', lower(@_user_id)), @_domain, HASHBYTES('SHA1', lower(@_domain)));

	
	select SCOPE_IDENTITY() as uid;	
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5GetUid')
DROP PROCEDURE [dbo].[TigSocks5GetUid]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5GetUid]
	@_user_id nvarchar(2049)
AS	
begin
	select uid from dbo.tig_socks5_users where sha1_user_id = HASHBYTES('SHA1', lower(@_user_id));
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5GetTransferLimits')
DROP PROCEDURE [dbo].[TigSocks5GetTransferLimits]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5GetTransferLimits]
	@_user_id nvarchar(2049)
AS
begin	
	select filesize_limit, transfer_limit_per_user, transfer_limit_per_domain from dbo.tig_socks5_users 
		where sha1_user_id = HASHBYTES('SHA1', lower(@_user_id));

end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5TransferUsedGeneral')
DROP PROCEDURE [dbo].[TigSocks5TransferUsedGeneral]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5TransferUsedGeneral]
AS
begin	
	select sum([transferred_bytes]) from [dbo].[tig_socks5_connections]
		where [transfer_timestamp] >  FORMAT(GETDATE(), 'yyyy-mm-01', 'en-US');
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5TransferUsedInstance')
DROP PROCEDURE [dbo].[TigSocks5TransferUsedInstance]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5TransferUsedInstance]
	@_instance nvarchar(128)
AS
begin
	select sum(transferred_bytes) from dbo.tig_socks5_connections
		where transfer_timestamp > FORMAT(GETDATE(), 'yyyy-mm-01', 'en-US') and instance = @_instance;
end
-- QUERY END:
GO

-- QUERY START:v
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5TransferUsedDomain')
DROP PROCEDURE [dbo].[TigSocks5TransferUsedDomain]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5TransferUsedDomain]
	@_domain nvarchar(2049)
AS
begin	
	select sum(transferred_bytes) from dbo.tig_socks5_connections
		where transfer_timestamp > FORMAT(GETDATE(), 'yyyy-mm-01', 'en-US')
		and [uid] in (select uid from dbo.tig_socks5_users where sha1_domain = HASHBYTES('SHA1', lower(@_domain)));
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5TransferUsedUser')
DROP PROCEDURE [dbo].[TigSocks5TransferUsedUser]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5TransferUsedUser]
	@_uid bigint
AS
begin	
	select sum(transferred_bytes) from dbo.tig_socks5_connections
		where transfer_timestamp > FORMAT(GETDATE(), 'yyyy-mm-01', 'en-US')
		and uid = @_uid;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5CreateTransferUsed')
DROP PROCEDURE [dbo].[TigSocks5CreateTransferUsed]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5CreateTransferUsed]
	@_uid bigint,
	@_direction int,
	@_instance nvarchar(128)
AS
begin	
	insert into dbo.tig_socks5_connections (uid, direction, instance) 
		values (@_uid, @_direction, @_instance);

	select SCOPE_IDENTITY() as uid;	
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigSocks5UpdateTransferUsed')
DROP PROCEDURE [dbo].[TigSocks5UpdateTransferUsed]
-- QUERY END:
GO

-- QUERY START:
create procedure [dbo].[TigSocks5UpdateTransferUsed]
	@_conn_id bigint,
	@_transferred_bytes bigint
AS
begin
	update dbo.tig_socks5_connections set transferred_bytes = @_transferred_bytes 
		where conn_id = @_conn_id;
end
-- QUERY END:
GO

