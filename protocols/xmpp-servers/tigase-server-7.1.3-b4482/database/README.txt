In order to use dedicated PubSub tables, you need to:

1. import ${database_type}-pubsub-schema.sql to Tigase ${database_type} DB
2. set Tigase configuration:
	pubsub/pubsub-repo-class = tigase.pubsub.repository.PubSubDAOJDBC
	pubsub/pubsub-repo-url = "database connection string, usually the same as for user repo"
