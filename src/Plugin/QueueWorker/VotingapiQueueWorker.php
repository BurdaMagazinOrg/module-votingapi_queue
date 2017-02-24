<?php

namespace Drupal\votingapi_queue\Plugin\QueueWorker;
use Drupal\Core\Cache\Cache;
use \Drupal\Core\Queue\QueueWorkerBase;
use Drupal\votingapi\VoteResultFunctionManager;

/**
 * The RenderWorker implementation class.
 *
 * @QueueWorker(
 *   id = "votingapi_queue",
 *   title = @Translation("Render queue")
 * )
 */
class VotingapiQueueWorker extends QueueWorkerBase {

  /**
   * {@inheritdoc}
   */
  public function processItem($data) {
    /**
     * @var VoteResultFunctionManager $manager
     */
    $manager = \Drupal::service('plugin.manager.votingapi.resultfunction');
    $manager->recalculateResults(
      $data['entity_type_id'],
      $data['entity_id'],
      $data['vote_type']
    );
    $cache_tag = $data['entity_type'] . ':' . $data['entity_id'];
    Cache::invalidateTags([$cache_tag]);
  }
}