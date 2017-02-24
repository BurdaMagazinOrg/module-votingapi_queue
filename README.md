# Votingapi Queue

This module provides a queue for updating voted entites. The configuration hooks into the default votingapi settings page. Choose manual on "tally results" settings. Additional settings for batch size and cron execution are available.

# Available Drush commands

(options available, see drush help for more information)

Start a new worker:

```~$ votingapi-queue-run```

Populate queue:

```~$ votingapi-queue-populate```

Reset:

```~$ votingapi-queue-set-last-run-time FALSE```

Debug/ Info:

```~$ votingapi-queue-debug```
