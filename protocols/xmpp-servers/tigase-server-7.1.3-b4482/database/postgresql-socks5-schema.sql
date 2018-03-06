-- QUERY START:
create table tig_socks5_users (
	-- unique uid for fast lookup
	uid bigserial,
	-- jid of user
	user_id varchar(2049) NOT NULL,

	-- domain part of jid of user
	"domain" varchar(2049) NOT NULL,

	-- limit of file size 
	filesize_limit bigint default 0,
	-- limit of transfer per user (0 - get default, -1 - deny any transfer)
	transfer_limit_per_user bigint default 0,
	-- limit of transfer per domain (0 - get default, -1 - deny any transfer)
	transfer_limit_per_domain bigint default 0,

	primary key (uid)
);

-- QUERY END:

-- QUERY START:
create unique index tig_socks5_users_user_id on tig_socks5_users ( user_id );
-- QUERY END:

-- QUERY START:
create unique index tig_socks5_users_domain on tig_socks5_users ( "domain" );
-- QUERY END:

-- QUERY START:
create table tig_socks5_connections (

	-- id of connection for fast lookup for update
	conn_id bigserial,

	-- uid of user (uid of jid)
	uid bigint NOT NULL references tig_socks5_users(uid),

        -- server instance used as proxy
        instance varchar(128) NOT NULL,

	-- direction of transfer,  -- 0-in, 1-out
	direction int NOT NULL,

	-- count of bytes transferred thru connections
	transferred_bytes bigint default 0,

	-- timestamp of last part transfer
	transfer_timestamp timestamp DEFAULT now(),

	primary key (conn_id)
);
-- QUERY END:

-- QUERY START:
create index tig_socks5_connections_uid on tig_socks5_connections ( uid );
-- QUERY END:

-- QUERY START:
create index tig_socks5_connections_uid_transfer_timestamp on tig_socks5_connections ( uid, transfer_timestamp );
-- QUERY END:

-- QUERY START:
create index tig_socks5_connections_instance_transfer_timestamp on tig_socks5_connections ( instance, transfer_timestamp );
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5CreateUid(varchar(2049), varchar(2049))
	returns bigint as '
declare
	_user_id alias for $1;
        _domain alias for $2;
	_res_uid bigint;
begin	
	insert into tig_socks5_users (user_id, "domain") 
		values (_user_id, _domain);

	select currval(''tig_socks5_users_uid_seq'') into _res_uid;

	return _res_uid as uid;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5GetUid(varchar(2049))
	returns bigint as '
declare 
	_user_id alias for $1;
	_res_uid bigint;
begin
	select uid into _res_uid from tig_socks5_users where user_id = _user_id;
	
	return _res_uid;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5GetTransferLimits(_user_id varchar(2049), OUT _filesize_limit bigint,
	OUT _transfer_limit_per_user bigint, OUT _transfer_limit_per_domain bigint) as '
	select filesize_limit, transfer_limit_per_user, transfer_limit_per_domain from tig_socks5_users 
		where user_id = $1;
'LANGUAGE 'sql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5TransferUsedGeneral()
	returns bigint as '
declare
	_res bigint;
begin	
	select sum(transferred_bytes) into _res from tig_socks5_connections
		where transfer_timestamp > to_char(now(), ''YYYY-MM-01'');

	return _res;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5TransferUsedInstance(varchar(128))
	returns bigint as '
declare
	_instance alias for $1;
	_res bigint;
begin	
	select sum(transferred_bytes) into _res from tig_socks5_connections
		where transfer_timestamp > to_char(now(), ''YYYY-MM-01'')
		and instance = _instance;

	return _res;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5TransferUsedDomain(varchar(2049))
	returns bigint as '
declare
	_domain alias for $1;
	_res bigint;
begin	
	select sum(transferred_bytes) into _res from tig_socks5_connections
		where transfer_timestamp > to_char(now(), ''YYYY-MM-01'')
		and uid in (select uid from tig_socks5_users where "domain" = _domain);

	return _res;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5TransferUsedUser(bigint)
	returns bigint as '
declare
	_uid alias for $1;
	_res bigint;
begin	
	select sum(transferred_bytes) into _res from tig_socks5_connections
		where transfer_timestamp > to_char(now(), ''YYYY-MM-01'')
		and uid = _uid;

	return _res;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5CreateTransferUsed(bigint, int, varchar(128))
	returns bigint as '
declare
	_uid alias for $1;
	_direction alias for $2;
        _instance alias for $3;
	_res bigint;
begin	
	insert into tig_socks5_connections (uid, direction, instance) 
		values (_uid, _direction, _instance);

	select currval(''tig_socks5_connections_conn_id_seq'') into _res;

	return _res;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

-- QUERY START:
create or replace function TigSocks5UpdateTransferUsed(bigint, bigint)
	returns void as '
declare
	_conn_id alias for $1;
	_transferred_bytes alias for $2;
begin
	update tig_socks5_connections set transferred_bytes = _transferred_bytes 
		where conn_id = _conn_id;
end;
' LANGUAGE 'plpgsql';
-- QUERY END:

