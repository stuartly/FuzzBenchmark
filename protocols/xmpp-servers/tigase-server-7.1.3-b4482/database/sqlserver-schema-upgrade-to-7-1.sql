--
--  Tigase Jabber/XMPP Server
--  Copyright (C) 2004-2016 "Tigase, Inc." <office@tigase.com>
--
--  This program is free software: you can redistribute it and/or modify
--  it under the terms of the GNU Affero General Public License as published by
--  the Free Software Foundation, either version 3 of the License.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU Affero General Public License for more details.
--
--  You should have received a copy of the GNU Affero General Public License
--  along with this program. Look for COPYING file in the top folder.
--  If not, see http://www.gnu.org/licenses/.
--
--

-- QUERY START:
SET QUOTED_IDENTIFIER ON
-- QUERY END:
GO


-- QUERY START:
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPutDBProperty')
DROP PROCEDURE TigPutDBProperty
-- QUERY END:
GO

-- QUERY START:
IF EXISTS(SELECT * FROM sys.indexes WHERE object_id = object_id('dbo.tig_pairs') AND NAME ='IX_clustered_tig_pairs_nid_uid_pkey')
    DROP INDEX IX_clustered_tig_pairs_nid_uid_pkey ON dbo.tig_pairs;
-- QUERY END:
GO



-- QUERY START:
-- Database properties set - procedure
create procedure dbo.TigPutDBProperty
	@_tkey nvarchar(255),
	@_tval ntext
	AS
	begin
		Declare @_nid int;
		Declare @_uid int;
		Declare @_count int;
		if exists (select 1 from dbo.tig_pairs, dbo.tig_users
					where (sha1_user_id = HASHBYTES('SHA1', LOWER(N'db-properties')))
						AND (dbo.tig_users.uid = dbo.tig_pairs.uid)  AND (pkey = @_tkey))
			begin
				select @_nid = dbo.tig_pairs.nid, @_uid = dbo.tig_pairs.uid from dbo.tig_pairs, dbo.tig_users
					where (sha1_user_id = HASHBYTES('SHA1', LOWER(N'db-properties')))
						AND (dbo.tig_users.uid = dbo.tig_pairs.uid)  AND (pkey = @_tkey);
				update dbo.tig_pairs set pval = @_tval
					where (@_uid = uid) AND (pkey = @_tkey) ;
			end
		else
			begin
				select @_nid = dbo.tig_pairs.nid, @_uid = dbo.tig_pairs.uid from dbo.tig_pairs, dbo.tig_users
					where (sha1_user_id = HASHBYTES('SHA1', LOWER(N'db-properties')))
						AND (dbo.tig_users.uid = dbo.tig_pairs.uid)  AND (pkey = @_tkey);
				insert into dbo.tig_pairs (pkey, pval, uid, nid)
					select @_tkey, @_tval, tu.uid, tn.nid from dbo.tig_users tu  left join tig_nodes tn on tn.uid=tu.uid
						where (sha1_user_id = HASHBYTES('SHA1', LOWER(N'db-properties')) and tn.node='root');
						
			end
	end
-- QUERY END:
GO



-- QUERY START:
ALTER TABLE [dbo].[tig_pairs]
   ADD [pid] [bigint] IDENTITY(1,1) NOT NULL,
   CONSTRAINT [PK_tig_pairs] PRIMARY KEY CLUSTERED ( [pid] ASC ) ON [PRIMARY]
-- QUERY END:
GO


-- QUERY START:
update tig_pairs set nid = (select nid from tig_nodes where uid=(select uid from tig_users where user_id='db-properties') and node='root') where uid=(select uid from tig_users where user_id='db-properties');
-- QUERY END:
GO


select GETDATE(), ' - Setting schema version to 7.1';

-- QUERY START:
exec dbo.TigPutDBProperty 'schema-version', '7.1';
-- QUERY END:
GO