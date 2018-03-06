-- renaming tables by adding suffix _1
alter table tig_pubsub_items rename to tig_pubsub_items_1;
alter table tig_pubsub_subscriptions rename to tig_pubsub_subscriptions_1;
alter table tig_pubsub_nodes rename to tig_pubsub_nodes_1;

-- droping procedures and functions
drop procedure if exists TigPubSubCreateNode;
drop procedure if exists TigPubSubRemoveNode;
drop procedure if exists TigPubSubGetItem;
drop procedure if exists TigPubSubWriteItem;
drop procedure if exists TigPubSubDeleteItem;
drop procedure if exists TigPubSubGetNodeItemsIds;
drop procedure if exists TigPubSubGetAllNodes;
drop procedure if exists TigPubSubDeleteAllNodes;
drop procedure if exists TigPubSubSetNodeConfiguration;
drop procedure if exists TigPubSubSetNodeAffiliations;
drop procedure if exists TigPubSubGetNodeConfiguration;
drop procedure if exists TigPubSubGetNodeAffiliations;
drop procedure if exists TigPubSubGetNodeSubscriptions;
drop procedure if exists TigPubSubSetNodeSubscriptions;
drop procedure if exists TigPubSubDeleteNodeSubscriptions;
drop function if exists substrCount;