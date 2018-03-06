-- QUERY START:
create table if not exists tig_socks5_users (
	-- unique uid for fast lookup
	uid bigint unsigned NOT NULL auto_increment,
	-- jid of user
	user_id varchar(2049) NOT NULL,
	-- sha1 hash of user_id for fast lookup
	sha1_user_id char(128) NOT NULL,

	-- domain part of jid of user
	`domain` varchar(2049) NOT NULL,
	-- sha1 hash of domain part of user_id for fast lookup
	sha1_domain char(128) NOT NULL,

	-- limit of file size 
	filesize_limit bigint default 0,
	-- limit of transfer per user (0 - get default, -1 - deny any transfer)
	transfer_limit_per_user bigint default 0,
	-- limit of transfer per domain (0 - get default, -1 - deny any transfer)
	transfer_limit_per_domain bigint default 0,

	primary key (uid),
	unique key sha1_user_id (sha1_user_id),
	key user_id (user_id(765))

)
ENGINE=InnoDB default character set utf8 ROW_FORMAT=DYNAMIC;
-- QUERY END:

-- QUERY START:
create table if not exists tig_socks5_connections (

	-- id of connection for fast lookup for update
	conn_id bigint unsigned NOT NULL auto_increment,

	-- uid of user (uid of jid)
	uid bigint unsigned NOT NULL,

        -- server instance used as proxy
        instance varchar(128) NOT NULL,

	-- direction of transfer,  -- 0-in, 1-out
	direction int NOT NULL,

	-- count of bytes transferred thru connections
	transferred_bytes bigint default 0,

	-- timestamp of last part transfer
	transfer_timestamp timestamp DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

	primary key (conn_id),
	key uid__transfer_timestamp (uid, transfer_timestamp),
        key instance__transfer_timestamp (instance, transfer_timestamp),
	foreign key (uid) references tig_socks5_users(uid)
)
ENGINE=InnoDB default character set utf8 ROW_FORMAT=DYNAMIC;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigSocks5CreateUid;
-- QUERY END:
-- QUERY START:
drop procedure if exists TigSocks5GetUid;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigSocks5GetTransferLimits;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigSocks5TransferUsedGeneral;
-- QUERY END:
-- QUERY START:
drop procedure if exists TigSocks5TransferUsedInstance;
-- QUERY END:
-- QUERY START:
drop procedure if exists TigSocks5TransferUsedDomain;
-- QUERY END:
-- QUERY START:
drop procedure if exists TigSocks5TransferUsedUser;
-- QUERY END:

-- QUERY START:
drop procedure if exists TigSocks5CreateTransferUsed;
-- QUERY END:
-- QUERY START:
drop procedure if exists TigSocks5UpdateTransferUsed;
-- QUERY END:


delimiter //
-- QUERY START:
create procedure TigSocks5CreateUid(_user_id varchar(2049) CHARSET utf8, _domain varchar(2049) CHARSET utf8)
begin	
	insert into tig_socks5_users (user_id, sha1_user_id, `domain`, sha1_domain) 
		values (_user_id, sha1(lower(_user_id)), _domain, sha1(lower(_domain)));

	select LAST_INSERT_ID() as uid;
end //
-- QUERY END:

-- QUERY START:
create procedure TigSocks5GetUid(_user_id varchar(2049) CHARSET utf8)
begin
	select uid from tig_socks5_users where sha1_user_id = sha1(lower(_user_id));
end //
-- QUERY END:

-- QUERY START:
create procedure TigSocks5GetTransferLimits(_user_id varchar(2049) CHARSET utf8)
begin	
	select filesize_limit, transfer_limit_per_user, transfer_limit_per_domain from tig_socks5_users 
		where sha1_user_id = sha1(lower(_user_id));

end //
-- QUERY END:

-- QUERY START:
create procedure TigSocks5TransferUsedGeneral()
begin	
	select sum(transferred_bytes) from tig_socks5_connections
		where transfer_timestamp > DATE_FORMAT(now(), '%y-%m-01');
end //
-- QUERY END:

-- QUERY START:
create procedure TigSocks5TransferUsedInstance(_instance varchar(128) CHARSET utf8)
begin	
	select sum(transferred_bytes) from tig_socks5_connections
		where transfer_timestamp > DATE_FORMAT(now(), '%y-%m-01') and instance = _instance;
end //
-- QUERY END:

-- QUERY START:v
create procedure TigSocks5TransferUsedDomain(_domain varchar(2049) CHARSET utf8)
begin	
	select sum(transferred_bytes) from tig_socks5_connections
		where transfer_timestamp > DATE_FORMAT(now(), '%y-%m-01')
		and uid in (select uid from tig_socks5_users where sha1_domain = sha1(lower(_domain)));
end //
-- QUERY END:

-- QUERY START:
create procedure TigSocks5TransferUsedUser(_uid bigint unsigned)
begin	
	select sum(transferred_bytes) from tig_socks5_connections
		where transfer_timestamp > DATE_FORMAT(now(), '%y-%m-01')
		and uid = _uid;
end //
-- QUERY END:

-- QUERY START:
create procedure TigSocks5CreateTransferUsed(_uid bigint unsigned, _direction int, _instance varchar(128) CHARSET utf8)
begin	
	insert into tig_socks5_connections (uid, direction, instance) 
		values (_uid, _direction, _instance);

	select LAST_INSERT_ID() as res_id;
end //
-- QUERY END:

-- QUERY START:
create procedure TigSocks5UpdateTransferUsed(_conn_id bigint unsigned, _transferred_bytes bigint unsigned)
begin
	update tig_socks5_connections set transferred_bytes = _transferred_bytes 
		where conn_id = _conn_id;
end //
-- QUERY END:


delimiter ;