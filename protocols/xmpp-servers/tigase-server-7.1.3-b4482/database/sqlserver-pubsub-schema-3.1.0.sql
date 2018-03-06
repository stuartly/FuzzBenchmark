-- LOAD FILE: database/sqlserver-pubsub-schema-3.0.0.sql

-- QUERY START:
SET QUOTED_IDENTIFIER ON
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
		SET publisher_id = @_publisher_id, data = @_item_data, update_date = getutcdate()
		WHERE tig_pubsub_items.node_id = @_node_id
			and tig_pubsub_items.id_index = CAST(@_item_id as nvarchar(255))
			and tig_pubsub_items.id = @_item_id;
	-- Insert the row if the UPDATE statement failed.
	IF (@@ROWCOUNT = 0 )
	BEGIN
		BEGIN TRY
				insert into tig_pubsub_items (node_id, id, id_sha1, creation_date, update_date, publisher_id, data)
				select @_node_id, @_item_id, HASHBYTES('SHA1',@_item_id), getutcdate(), getutcdate(), @_publisher_id, @_item_data where not exists(
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
IF EXISTS (SELECT * FROM sys.objects WHERE type = 'P' AND name = 'TigPubSubRemoveService')
	DROP PROCEDURE TigPubSubRemoveService
-- QUERY END:
GO

-- QUERY START:
create procedure dbo.TigPubSubRemoveService
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
	delete from tig_pubsub_service_jids where service_id = @_service_id;
	delete from tig_pubsub_affiliations where jid_id in (
		select j.jid_id from tig_pubsub_jids j where j.jid_sha1 = HASHBYTES('SHA1', @_service_jid) and j.jid = @_service_jid);
	delete from tig_pubsub_subscriptions where jid_id in (
		select j.jid_id from tig_pubsub_jids j where j.jid_sha1 = HASHBYTES('SHA1', @_service_jid) and j.jid = @_service_jid);
end
-- QUERY END:
GO

