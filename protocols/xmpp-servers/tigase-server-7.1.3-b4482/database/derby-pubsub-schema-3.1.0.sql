run 'database/derby-pubsub-schema-3.0.0.sql';

-- LOAD FILE: database/derby-pubsub-schema-3.0.0.sql

-- QUERY START:
create procedure TigPubSubRemoveService(service_jid varchar(2049))
	PARAMETER STYLE JAVA
	LANGUAGE JAVA
	MODIFIES SQL DATA
	DYNAMIC RESULT SETS 1
	EXTERNAL NAME 'tigase.pubsub.repository.derby.StoredProcedures.tigPubSubRemoveService';
-- QUERY END: