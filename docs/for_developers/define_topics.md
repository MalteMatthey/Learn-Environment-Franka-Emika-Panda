
# Creating New Topics for Tasks

This guide will walk you through the process of creating and integrating new task topics in the learn environment.

### Defining Topics

All topics are defined in the JSON file located at `/task_pool/topic_definitions.json`. To add new topics, you simply need to modify this file.

### Example Configuration

Here's an example of how the `topic_definitions.json` file might look:

```json
{
  "topics": [
    "ROS",
    "MoveIt",
    "Controllers"
  ]
}

```

In this example, three topics are defined: `ROS`, `MoveIt`, and `Controllers`. You can add more topics to this list as needed.

### Using Topics in Task Definitions

Once you have defined new topics in the `topic_definitions.json` file, you can use them in your `task_definitions.json` configuration. Simply reference any of the defined topics for your tasks.

### Default Topic Behavior

If you add a topic to a task which is not defined in `topic_definitions.json`, the system will automatically assign the `unknown` topic. To avoid this, make sure all topics used in tasks are properly defined in the `topic_definitions.json` file.

### Task Ordering

Tasks in the `task_definitions.json` file are grouped by their associated topics. The order of topics is determined by the order in which they appear in the `topic_definitions.json` file. Within each topic, tasks are displayed in the order they are defined.