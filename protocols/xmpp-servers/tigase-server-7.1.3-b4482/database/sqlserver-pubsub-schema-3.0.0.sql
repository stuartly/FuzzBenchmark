-- QUERY START:
SET QUOTED_IDENTIFIER ON
-- QUERY END:
GO

-- QUERY START:
if not exists (select * from sysobjects where name='tig_pubsub_service_jids' and xtype='U')
	CREATE  TABLE [dbo].[tig_pubsub_service_jids] (
		[service_id] [bigint] IDENTITY(1,1) NOT NULL,
		[service_jid] [nvarchar](2049) NOT NULL,
		[service_jid_sha1] [varbinary](40) NOT NULL,
		[service_jid_index] AS CAST( [service_jid] AS NVARCHAR(255)),
		PRIMARY KEY ( [service_id] ),
		UNIQUE (service_jid_sha1)
	);
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_service_jids') AND NAME ='IX_tig_pubsub_service_jids_service_jid')
	CREATE INDEX IX_tig_pubsub_service_jids_service_jid ON [dbo].[tig_pubsub_service_jids](service_jid_index);
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_service_jids') AND NAME ='IX_tig_pubsub_service_jids_service_jids_sha1')
	CREATE INDEX IX_tig_pubsub_service_jids_service_jids_sha1 ON [dbo].[tig_pubsub_service_jids](service_jid_sha1);
-- QUERY END:
GO

-- QUERY START:
if not exists (select * from sysobjects where name='tig_pubsub_jids' and xtype='U')
	CREATE  TABLE [dbo].[tig_pubsub_jids] (
		[jid_id] [bigint] IDENTITY(1,1) NOT NULL,
		[jid] [nvarchar](2049) NOT NULL,
		[jid_sha1] [varbinary](40) NOT NULL,
		[jid_index] AS CAST( [jid] AS NVARCHAR(255)),
		PRIMARY KEY ( [jid_id] ),
		UNIQUE (jid_sha1)
	);
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_jids') AND NAME ='IX_tig_pubsub_jids_jid')
	CREATE INDEX IX_tig_pubsub_jids_jid ON [dbo].[tig_pubsub_jids](jid_index);
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_jids') AND NAME ='IX_tig_pubsub_jids_jid_sha1')
	CREATE INDEX IX_tig_pubsub_jids_jid_sha1 ON [dbo].[tig_pubsub_jids](jid_sha1);
-- QUERY END:
GO

-- QUERY START:
if not exists (select * from sysobjects where name='tig_pubsub_nodes' and xtype='U')
	CREATE  TABLE [dbo].[tig_pubsub_nodes] (
		[node_id] [bigint] IDENTITY(1,1) NOT NULL,
		[service_id] [bigint] NOT NULL,
		[name] [nvarchar](1024) NOT NULL,
		[name_sha1] [varbinary](40) NOT NULL,
		[name_index] AS CAST( [name] as NVARCHAR(255)),
		[type] [int] NOT NULL,
		[title] [nvarchar](1000),
		[description] [nvarchar](MAX),
		[creator_id] [bigint],
		[creation_date] [datetime],
		[configuration] [nvarchar](MAX),
		[collection_id] [bigint],
		PRIMARY KEY (node_id),

		CONSTRAINT [FK_tig_pubsub_nodes_service_id] FOREIGN KEY ([service_id])
			REFERENCES [dbo].[tig_pubsub_service_jids]([service_id])
			ON DELETE CASCADE,
		CONSTRAINT [FK_tig_pubsub_nodes_creator_id] FOREIGN KEY ([creator_id])
			REFERENCES [dbo].[tig_pubsub_jids](jid_id),
		CONSTRAINT [FK_tig_pubsub_nodes_collection_id] FOREIGN KEY ([collection_id])
			REFERENCES [dbo].[tig_pubsub_nodes](node_id),
		UNIQUE (service_id, name_sha1)
	);
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_nodes') AND NAME ='IX_tig_pubsub_nodes_service_id')
	CREATE INDEX IX_tig_pubsub_nodes_service_id ON [dbo].[tig_pubsub_nodes](service_id);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_nodes') AND NAME ='IX_tig_pubsub_nodes_name')
	CREATE INDEX IX_tig_pubsub_nodes_name ON [dbo].[tig_pubsub_nodes](name_index);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_nodes') AND NAME ='IX_tig_pubsub_nodes_service_id_name')
	CREATE INDEX IX_tig_pubsub_nodes_service_id_name ON [dbo].[tig_pubsub_nodes](service_id,name_index);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_nodes') AND NAME ='IX_tig_pubsub_nodes_collection_id')
	CREATE INDEX IX_tig_pubsub_nodes_collection_id ON [dbo].[tig_pubsub_nodes](collection_id);
-- QUERY END:
GO

-- QUERY START:
if not exists (select * from sysobjects where name='tig_pubsub_affiliations' and xtype='U')
	CREATE  TABLE [dbo].[tig_pubsub_affiliations] (
		[node_id] [bigint] NOT NULL,
		[jid_id] [bigint] NOT NULL,
		[affiliation] [nvarchar](20) NOT NULL,
		PRIMARY KEY (node_id, jid_id),

		CONSTRAINT [FK_tig_pubsub_affiliations_node_id] FOREIGN KEY ([node_id])
			REFERENCES [dbo].[tig_pubsub_nodes](node_id),
		CONSTRAINT [FK_tig_pubsub_affiliations_jid_id] FOREIGN KEY ([jid_id])
			REFERENCES [dbo].[tig_pubsub_jids](jid_id)
	);
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_affiliations') AND NAME ='IX_tig_pubsub_affiliations_node_id')
	CREATE INDEX IX_tig_pubsub_affiliations_node_id ON [dbo].[tig_pubsub_affiliations](node_id);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_affiliations') AND NAME ='IX_tig_pubsub_affiliations_jid_id')
	CREATE INDEX IX_tig_pubsub_affiliations_jid_id ON [dbo].[tig_pubsub_affiliations](jid_id);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_affiliations') AND NAME ='IX_tig_pubsub_affiliations_node_id_jid_id')
	CREATE INDEX IX_tig_pubsub_affiliations_node_id_jid_id ON [dbo].[tig_pubsub_affiliations](node_id, jid_id);
-- QUERY END:
GO

-- QUERY START:
if not exists (select * from sysobjects where name='tig_pubsub_subscriptions' and xtype='U')
	CREATE  TABLE [dbo].[tig_pubsub_subscriptions] (
		[node_id] [bigint] NOT NULL,
		[jid_id] [bigint] NOT NULL,
		[subscription] [nvarchar](20) NOT NULL,
		[subscription_id] [nvarchar](40) NOT NULL,
		PRIMARY KEY (node_id, jid_id),

		CONSTRAINT [FK_tig_pubsub_subcriptions_node_id] FOREIGN KEY ([node_id])
			REFERENCES [dbo].[tig_pubsub_nodes](node_id),
		CONSTRAINT [FK_tig_pubsub_subscriptions_jid_id] FOREIGN KEY ([jid_id])
			REFERENCES [dbo].[tig_pubsub_jids](jid_id)
	);
-- QUERY END:
GO


-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_subscriptions') AND NAME ='IX_tig_pubsub_subscriptions_node_id')
	CREATE INDEX IX_tig_pubsub_subscriptions_node_id ON [dbo].[tig_pubsub_subscriptions](node_id);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_subscriptions') AND NAME ='IX_tig_pubsub_subscriptions_jid_id')
	CREATE INDEX IX_tig_pubsub_subscriptions_jid_id ON [dbo].[tig_pubsub_subscriptions](jid_id);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_subscriptions') AND NAME ='IX_tig_pubsub_subscription_node_id_jid_id')
	CREATE INDEX IX_tig_pubsub_subscription_node_id_jid_id ON [dbo].[tig_pubsub_subscriptions](node_id, jid_id);
-- QUERY END:
GO

-- QUERY START:
if not exists (select * from sysobjects where name='tig_pubsub_items' and xtype='U')
	CREATE  TABLE [dbo].[tig_pubsub_items] (
		[node_id] [bigint] NOT NULL,
		[id] [nvarchar](1024) NOT NULL,
		[id_sha1] [varbinary](40) NOT NULL,
		[id_index] AS CAST( [id] AS NVARCHAR(255)),
		[creation_date] [datetime],
		[publisher_id] [bigint],
		[update_date] [datetime],
		[data] [nvarchar](MAX),
		PRIMARY KEY (node_id,id_sha1),

		CONSTRAINT [FK_tig_pubsub_items_node_id] FOREIGN KEY ([node_id])
			REFERENCES [dbo].[tig_pubsub_nodes](node_id),
		CONSTRAINT [FK_tig_pubsub_items_publisher_id] FOREIGN KEY ([publisher_id])
			REFERENCES [dbo].[tig_pubsub_jids](jid_id)
	);
-- QUERY END:
GO

-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_items') AND NAME ='IX_tig_pubsub_items_node_id')
	CREATE INDEX IX_tig_pubsub_items_node_id ON [dbo].[tig_pubsub_items](node_id);
-- QUERY END:
GO
-- QUERY START:
IF NOT EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pubsub_items') AND NAME ='IX_tig_pubsub_items_node_id_id')
	CREATE INDEX IX_tig_pubsub_items_node_id_id ON [dbo].[tig_pubsub_items](node_id, id_index);
-- QUERY END:
GO

-- PROCEDURES

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubEnsureServiceJid')
	DROP PROCEDURE TigPubSubEnsureServiceJid
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubEnsureServiceJid
	@_service_jid nvarchar(2049),
	@_service_id bigint OUTPUT
AS
begin
	declare @_service_jid_sha1 varbinary(20);

		set @_service_jid_sha1 = HASHBYTES('SHA1', @_service_jid);
		select @_service_id=service_id from tig_pubsub_service_jids
			where service_jid_sha1 = @_service_jid_sha1 and service_jid = @_service_jid;
		if @_service_id is null
		begin
	BEGIN TRY
			insert into tig_pubsub_service_jids (service_jid,service_jid_sha1)
				select @_service_jid, @_service_jid_sha1 where not exists(
							select 1 from tig_pubsub_service_jids where service_jid_sha1 = @_service_jid_sha1 and service_jid = @_service_jid);
			set @_service_id = @@IDENTITY
			END TRY
			BEGIN CATCH
					IF ERROR_NUMBER() = 2627
						select @_service_id=service_id from tig_pubsub_service_jids
							where service_jid_sha1 = @_service_jid_sha1 and service_jid = @_service_jid;
					ELSE
						declare @ErrorMessage nvarchar(max), @ErrorSeverity int, @ErrorState int;
						select @ErrorMessage = ERROR_MESSAGE() + ' Line ' + cast(ERROR_LINE() as nvarchar(5)), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();
						raiserror (@ErrorMessage, @ErrorSeverity, @ErrorState);
			END CATCH
		end
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubEnsureJid')
	DROP PROCEDURE TigPubSubEnsureJid
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubEnsureJid
	@_jid nvarchar(2049),
	@_jid_id bigint OUTPUT
AS
begin
	declare @_jid_sha1 varbinary(20);

		set @_jid_sha1 = HASHBYTES('SHA1', @_jid);
		select @_jid_id=jid_id from tig_pubsub_jids
			where jid_sha1 = @_jid_sha1 and jid = @_jid;
		if @_jid_id is null
		begin
			BEGIN TRY
			insert into tig_pubsub_jids (jid,jid_sha1)
				select @_jid, @_jid_sha1 where not exists(
							select 1 from tig_pubsub_jids where jid_sha1 = @_jid_sha1 and jid = @_jid);
			set @_jid_id = @@IDENTITY
			END TRY
			BEGIN CATCH
					IF ERROR_NUMBER() = 2627
						select @_jid_id=jid_id from tig_pubsub_jids
							where jid_sha1 = @_jid_sha1 and jid = @_jid;
					ELSE
						declare @ErrorMessage nvarchar(max), @ErrorSeverity int, @ErrorState int;
						select @ErrorMessage = ERROR_MESSAGE() + ' Line ' + cast(ERROR_LINE() as nvarchar(5)), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();
						raiserror (@ErrorMessage, @ErrorSeverity, @ErrorState);
			END CATCH
		end
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubCreateNode')
	DROP PROCEDURE TigPubSubCreateNode
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubCreateNode
	@_service_jid nvarchar(2049), 
	@_node_name nvarchar(1024),
	@_node_type int,
	@_node_creator nvarchar(2049),
	@_node_conf nvarchar(max),
	@_collection_id bigint
AS	
begin
	declare @_service_id bigint;
	declare @_node_creator_id bigint;

	exec TigPubSubEnsureServiceJid @_service_jid=@_service_jid, @_service_id=@_service_id output;
	exec TigPubSubEnsureJid @_jid=@_node_creator, @_jid_id=@_node_creator_id output;

	BEGIN TRY
		insert into dbo.tig_pubsub_nodes (service_id, name, name_sha1, type, creator_id, creation_date, configuration, collection_id)
				select @_service_id, @_node_name, HASHBYTES('SHA1', @_node_name), @_node_type, @_node_creator_id, getdate(), @_node_conf, @_collection_id where not exists(
							select 1 from tig_pubsub_nodes where service_id=@_service_id AND name_sha1=HASHBYTES('SHA1', @_node_name));

		select @@IDENTITY as node_id;

  END TRY
  BEGIN CATCH
      IF ERROR_NUMBER() = 2627
				select node_id from tig_pubsub_nodes where service_id=@_service_id AND name_sha1=HASHBYTES('SHA1', @_node_name)
			ELSE
					declare @ErrorMessage nvarchar(max), @ErrorSeverity int, @ErrorState int;
					select @ErrorMessage = ERROR_MESSAGE() + ' Line ' + cast(ERROR_LINE() as nvarchar(5)), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();
					raiserror (@ErrorMessage, @ErrorSeverity, @ErrorState);
  END CATCH

end
-- QUERY END:
GO


-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubRemoveNode')
	DROP PROCEDURE TigPubSubRemoveNode
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubRemoveNode
	@_node_id bigint
AS	
begin
  delete from dbo.tig_pubsub_items where node_id = @_node_id;
  delete from dbo.tig_pubsub_subscriptions where node_id = @_node_id;
  delete from dbo.tig_pubsub_affiliations where node_id = @_node_id;
  delete from dbo.tig_pubsub_nodes where node_id = @_node_id;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetItem')
	DROP PROCEDURE TigPubSubGetItem
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetItem
	@_node_id bigint,
	@_item_id nvarchar(1024)
AS	
begin
  select data, p.jid as publisher, creation_date, update_date
    from dbo.tig_pubsub_items pit
	inner join tig_pubsub_jids p on p.jid_id = pit.publisher_id
	where node_id = @_node_id AND id = @_item_id;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubWriteItem')
	DROP PROCEDURE TigPubSubWriteItem
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubWriteItem
	@_node_id bigint,
	@_item_id nvarchar(1024),
	@_publisher nvarchar(2049),
	@_item_data ntext
AS
begin
    SET NOCOUNT ON;
	declare @_publisher_id bigint;

	exec TigPubSubEnsureJid @_jid=@_publisher, @_jid_id=@_publisher_id output;
	-- Update the row if it exists.
    UPDATE tig_pubsub_items
		SET publisher_id = @_publisher_id, data = @_item_data, update_date = getdate()
		WHERE tig_pubsub_items.node_id = @_node_id
			and tig_pubsub_items.id_index = CAST(@_item_id as nvarchar(255))
			and tig_pubsub_items.id = @_item_id;
	-- Insert the row if the UPDATE statement failed.
	IF (@@ROWCOUNT = 0 )
	BEGIN
		BEGIN TRY
				insert into tig_pubsub_items (node_id, id, id_sha1, creation_date, update_date, publisher_id, data)
				select @_node_id, @_item_id, HASHBYTES('SHA1',@_item_id), getdate(), getdate(), @_publisher_id, @_item_data where not exists(
					select 1 from tig_pubsub_items where node_id = @_node_id AND id_sha1 = HASHBYTES('SHA1',@_item_id));
		END TRY
		BEGIN CATCH
				IF ERROR_NUMBER() <> 2627
						declare @ErrorMessage nvarchar(max), @ErrorSeverity int, @ErrorState int;
						select @ErrorMessage = ERROR_MESSAGE() + ' Line ' + cast(ERROR_LINE() as nvarchar(5)), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();
						raiserror (@ErrorMessage, @ErrorSeverity, @ErrorState);
		END CATCH
	END
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubDeleteItem')
	DROP PROCEDURE TigPubSubDeleteItem
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubDeleteItem
	@_node_id bigint,
	@_item_id nvarchar(1024)
AS	
begin
	delete from tig_pubsub_items where node_id = @_node_id 
		and id_index = CAST(@_item_id as NVARCHAR(255)) AND id = @_item_id ;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeId')
	DROP PROCEDURE TigPubSubGetNodeId
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeId
	@_service_jid nvarchar(2049),
	@_node_name nvarchar(1024)
AS
begin
	select n.node_id from tig_pubsub_nodes n
		inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
		where sj.service_jid_index = CAST(@_service_jid as NVARCHAR(255)) 
			and n.name_index = CAST(@_node_name as NVARCHAR(255))
			and sj.service_jid = @_service_jid and n.name = @_node_name;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeItemsIds')
	DROP PROCEDURE TigPubSubGetNodeItemsIds
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeItemsIds
	@_node_id bigint
AS	
begin
	select id from tig_pubsub_items where node_id = @_node_id order by creation_date;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeItemsIdsSince')
	DROP PROCEDURE TigPubSubGetNodeItemsIdsSince
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeItemsIdsSince
	@_node_id bigint,
	@_since datetime
AS	
begin
	select id from tig_pubsub_items where node_id = @_node_id 
		and creation_date >= @_since order by creation_date;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetAllNodes')
	DROP PROCEDURE TigPubSubGetAllNodes
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetAllNodes
	@_service_jid nvarchar(2049)
AS	
begin
	select n.name, n.node_id from tig_pubsub_nodes n
		inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
		where service_jid_sha1 = HASHBYTES('SHA1', @_service_jid);
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetRootNodes')
	DROP PROCEDURE TigPubSubGetRootNodes
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetRootNodes
	@_service_jid nvarchar(2049)
AS	
begin
	select n.name, n.node_id from tig_pubsub_nodes n
		inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id
		where service_jid_sha1 = HASHBYTES('SHA1', @_service_jid)
			and n.collection_id is null;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetChildNodes')
	DROP PROCEDURE TigPubSubGetChildNodes
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetChildNodes
	@_service_jid nvarchar(2049),
	@_node_name nvarchar(1024)
AS	
begin
	select n.name, n.node_id from tig_pubsub_nodes n 
		inner join tig_pubsub_service_jids sj on n.service_id = sj.service_id 
		inner join tig_pubsub_nodes p on p.node_id = n.collection_id and p.service_id = sj.service_id
		where sj.service_jid_sha1 = HASHBYTES('SHA1', @_service_jid) and p.name_index = CAST(@_node_name as NVARCHAR(255))
			and sj.service_jid = @_service_jid and p.name = @_node_name;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubDeleteAllNodes')
	DROP PROCEDURE TigPubSubDeleteAllNodes
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubDeleteAllNodes
	@_service_jid nvarchar(2049)
AS	
begin
	declare @_service_id bigint;
  
	select @_service_id=service_id from tig_pubsub_service_jids where service_jid_sha1 = HASHBYTES('SHA1', @_service_jid);
  
	delete from dbo.tig_pubsub_items where node_id in (
		select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);
	delete from dbo.tig_pubsub_subscriptions where node_id in (
		select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);;
	delete from dbo.tig_pubsub_affiliations where node_id in (
		select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);
	delete from dbo.tig_pubsub_nodes where node_id in (
		select n.node_id from tig_pubsub_nodes n where n.service_id = @_service_id);
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubSetNodeConfiguration')
	DROP PROCEDURE TigPubSubSetNodeConfiguration
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubSetNodeConfiguration
	@_node_id bigint,
	@_node_conf ntext,
	@_collection_id bigint
AS	
begin
	update tig_pubsub_nodes set configuration = @_node_conf, collection_id = @_collection_id
		where node_id = @_node_id;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubSetNodeAffiliation')
	DROP PROCEDURE TigPubSubSetNodeAffiliation
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubSetNodeAffiliation
	@_node_id bigint,
	@_jid nvarchar(2049),
	@_affil nvarchar(20)
AS
begin
	declare @_jid_id bigint;
	declare @_exists int;

	select @_jid_id = jid_id from tig_pubsub_jids where jid_index = CAST(@_jid as NVARCHAR(255)) and jid = @_jid;
	if @_jid_id is not null
		select @_exists = 1 from tig_pubsub_affiliations where node_id = @_node_id and jid_id = @_jid_id;
	if @_affil != 'none'
	begin
			if @_jid_id is null
				exec TigPubSubEnsureJid @_jid=@_jid, @_jid_id=@_jid_id output;
			if @_exists is not null
				update tig_pubsub_affiliations set affiliation = @_affil where node_id = @_node_id and jid_id = @_jid_id;
			else
				BEGIN TRY
					insert into tig_pubsub_affiliations (node_id, jid_id, affiliation)
					select @_node_id, @_jid_id, @_affil where not exists(
						select 1 from tig_pubsub_affiliations where node_id = @_node_id AND jid_id = @_jid_id);
				END TRY
				BEGIN CATCH
						IF ERROR_NUMBER() <> 2627
						declare @ErrorMessage nvarchar(max), @ErrorSeverity int, @ErrorState int;
						select @ErrorMessage = ERROR_MESSAGE() + ' Line ' + cast(ERROR_LINE() as nvarchar(5)), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();
						raiserror (@ErrorMessage, @ErrorSeverity, @ErrorState);
				END CATCH
		end
	else
	begin
		if @_exists is not null
			delete from tig_pubsub_affiliations where node_id = @_node_id and jid_id = @_jid_id;
	end
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeConfiguration')
	DROP PROCEDURE TigPubSubGetNodeConfiguration
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeConfiguration
	@_node_id bigint
AS
begin
  select configuration from tig_pubsub_nodes where node_id = @_node_id;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeAffiliations')
	DROP PROCEDURE TigPubSubGetNodeAffiliations
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeAffiliations
	@_node_id bigint
AS
begin
	select pj.jid, pa.affiliation from tig_pubsub_affiliations pa 
		inner join tig_pubsub_jids pj on pa.jid_id = pj.jid_id
		where pa.node_id = @_node_id;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeSubscriptions')
	DROP PROCEDURE TigPubSubGetNodeSubscriptions
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeSubscriptions
	@_node_id bigint
AS
begin
	select pj.jid, ps.subscription, ps.subscription_id 
		from tig_pubsub_subscriptions ps 
		inner join tig_pubsub_jids pj on ps.jid_id = pj.jid_id
		where ps.node_id = @_node_id;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubSetNodeSubscription')
	DROP PROCEDURE TigPubSubSetNodeSubscription
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubSetNodeSubscription
	@_node_id bigint,
	@_jid nvarchar(2049),
	@_subscr nvarchar(20),
	@_subscr_id nvarchar(40)
AS	
begin
    SET NOCOUNT ON;
	declare @_jid_id bigint;
	exec TigPubSubEnsureJid @_jid=@_jid, @_jid_id=@_jid_id output;
	-- Update the row if it exists.    
    UPDATE tig_pubsub_subscriptions
		SET subscription = @_subscr where node_id = @_node_id and jid_id = @_jid_id;
	-- Insert the row if the UPDATE statement failed.	
	IF (@@ROWCOUNT = 0 )
	BEGIN
		BEGIN TRY
			insert into tig_pubsub_subscriptions (node_id, jid_id, subscription, subscription_id)
				select @_node_id, @_jid_id, @_subscr, @_subscr_id where not exists(
					select 1 from tig_pubsub_subscriptions where node_id = @_node_id AND jid_id = @_jid_id);
		END TRY
		BEGIN CATCH
				IF ERROR_NUMBER() <> 2627
						declare @ErrorMessage nvarchar(max), @ErrorSeverity int, @ErrorState int;
						select @ErrorMessage = ERROR_MESSAGE() + ' Line ' + cast(ERROR_LINE() as nvarchar(5)), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();
						raiserror (@ErrorMessage, @ErrorSeverity, @ErrorState);
		END CATCH
	END
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubDeleteNodeSubscription')
	DROP PROCEDURE TigPubSubDeleteNodeSubscription
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubDeleteNodeSubscription
	@_node_id bigint,
	@_jid nvarchar(2049)
AS
begin
	delete from tig_pubsub_subscriptions where node_id = @_node_id and jid_id = (
		select jid_id from tig_pubsub_jids where jid_sha1 = HASHBYTES('SHA1', @_jid) and jid = @_jid);
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetUserAffiliations')
	DROP PROCEDURE TigPubSubGetUserAffiliations
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetUserAffiliations
	@_service_jid nvarchar(2049),
	@_jid nvarchar(2049)
AS
begin
	select n.name, pa.affiliation from tig_pubsub_nodes n 
		inner join tig_pubsub_service_jids sj on sj.service_id = n.service_id
		inner join tig_pubsub_affiliations pa on pa.node_id = n.node_id
		inner join tig_pubsub_jids pj on pj.jid_id = pa.jid_id
		where pj.jid_sha1 = HASHBYTES('SHA1',@_jid) and sj.service_jid_sha1 = HASHBYTES('SHA1',@_service_jid)
			and pj.jid = @_jid and sj.service_jid = @_service_jid;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetUserSubscriptions')
	DROP PROCEDURE TigPubSubGetUserSubscriptions
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetUserSubscriptions
	@_service_jid nvarchar(2049),
	@_jid nvarchar(2049)
AS
begin
	select n.name, ps.subscription, ps.subscription_id from tig_pubsub_nodes n 
		inner join tig_pubsub_service_jids sj on sj.service_id = n.service_id
		inner join tig_pubsub_subscriptions ps on ps.node_id = n.node_id
		inner join tig_pubsub_jids pj on pj.jid_id = ps.jid_id
		where pj.jid_sha1 = HASHBYTES('SHA1',@_jid) and sj.service_jid_sha1 = HASHBYTES('SHA1',@_service_jid)
			and pj.jid = @_jid and sj.service_jid = @_service_jid;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubGetNodeItemsMeta')
	DROP PROCEDURE TigPubSubGetNodeItemsMeta
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubGetNodeItemsMeta
	@_node_id bigint
AS
begin
	select id, creation_date, update_date from tig_pubsub_items where node_id = @_node_id order by creation_date;	
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubFixNode')
	DROP PROCEDURE TigPubSubFixNode
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubFixNode
	@_node_id bigint,
	@_creation_date datetime
AS
begin
	update tig_pubsub_nodes set creation_date = @_creation_date where node_id = @_node_id;
end
-- QUERY END:
GO

-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubFixItem')
	DROP PROCEDURE TigPubSubFixItem
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubFixitem
	@_node_id bigint,
	@_item_id nvarchar(1024),
	@_creation_date datetime,
	@_update_date datetime
AS
begin
	update tig_pubsub_items set creation_date = @_creation_date, update_date = @_update_date
		where node_id = @_node_id and id_index = CAST(@_item_id as NVARCHAR(255)) and id = @_item_id;
end
-- QUERY END:
GO