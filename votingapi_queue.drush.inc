<?php

/**
 * @file
 * Drush commands for votingapi_queue.
 */

use \Drupal\Core\Queue\QueueInterface;
use \Drupal\Core\Queue\QueueWorkerInterface;

/**
 * Implements hook_drush_command().
 */
function votingapi_queue_drush_command() {
  $commands = array();

  $commands['votingapi-queue-run'] = [
    'description' => dt('Starts a new worker process for the votingapi Queue.'),
    'core' => ['8+'],
    'options' => [
      'time-limit' => dt('The time limit. Default is set to value 0, which is unlimited.'),
    ],
  ];
  $commands['votingapi-queue-set-last-cron-time'] = [
    'description' => dt('Sets the last executed cron time.'),
    'core' => ['8+'],
    'arguments' => [
      'timestamp' => dt('The timestamp (integer) to be set. Pass string FALSE to unset the time.')
    ]
  ];
  $commands['votingapi-queue-debug'] = [
    'description' => dt('Prints some health and debug info. Containing last execution timestamp and page.'),
    'core' => ['8+'],
  ];

  return $commands;
}

/**
 * Processes the votingapi queue.
 */
function drush_votingapi_queue_run() {
  $time_limit = drush_get_option('time-limit', 0);
  $queue = \Drupal::queue('votingapi_queue');
  $queue_worker = \Drupal::service('plugin.manager.queue_worker')
    ->createInstance('votingapi_queue');

  if ($time_limit) {
    $end = time() + $time_limit;
    while (time() < $end) {
      _votingapi_queue_process_next_item($queue, $queue_worker, $time_limit);
    }
  }
  else {
    while (TRUE) {
      _votingapi_queue_process_next_item($queue, $queue_worker);
    }
  }

  drush_print("Finished votingapi queue run.");
}

/**
 * Helper function to process the next available item.
 *
 * When no item could be claimed,
 * a sleep for one second will be invoked
 * to reduce the possible overhead of database calls.
 */
function _votingapi_queue_process_next_item(QueueInterface $queue, QueueWorkerInterface $queue_worker, $lease_time = 30) {
  if ($item = $queue->claimItem($lease_time)) {
    try {
      // Remove the item from the queue,
      // no matter whether it has been processed successfully.
      $queue->deleteItem($item);
      $queue_worker->processItem($item->data);
    }
    catch (\Exception $e) {
      watchdog_exception('cron', $e);
    }
  }
  else {
    sleep(1);
  }
}

function drush_votingapi_queue_set_last_cron_time($timestamp) {
  $state = Drupal::state();
  if (strtoupper($timestamp) == 'FALSE') {
    $state->delete('votingapi.queue_next_page');
    drush_print(dt("Deleted votingapi.queue_next_page."));
    $state->delete('votingapi.queue_last_cron');
    drush_print(dt("Deleted votingapi.queue_last_cron."));
  } else {
    $state->delete('votingapi.queue_next_page');
    drush_print(dt("Deleted votingapi.queue_next_page."));
    $state->set('votingapi.queue_last_cron', $timestamp);
    drush_print(dt("set votingapi.queue_last_cron to $timestamp"));
  }

}

function drush_votingapi_queue_set_last_cron_time_validate() {
  $args_count = func_num_args();
  if ($args_count == 0) {
    return drush_set_error('ARGUMENTS_REQUIRED', dt('Please provide timestamp or FALSE as argument.'));
  } else {
    $timestamp = func_get_arg(0);
  }

  if (strtoupper($timestamp) == 'FALSE') {
    return TRUE;
  }
  if (!is_int($timestamp)) {
    return drush_set_error('ARGUMENTS_REQUIRED', dt('Please provide integer timestmap or FALSE string as argument.'));
  }
}

function drush_votingapi_queue_debug() {
  $state = Drupal::state();
  drush_print("votingapi.queue_last_cron:" . $state->get('votingapi.queue_last_cron'));
  drush_print("votingapi.queue_next_page:" . $state->get('votingapi.queue_next_page'));
}