<?php

  class sourcecontrol_Push {

    protected $binary;

    public function __construct() {

      global $conf;

      $bin = $conf['deploy']['push']['type'];
      $this->binary = trim(`which {$bin}`);

      if (!$this->binary)
        throw new Exception("Cannot locate push binary \"{$bin}\"");

    }


    public static function factory($type) {
      switch ($type) {
        case 'rsync':
          return new sourcecontrol_Push_Rsync();
        default:
          throw new Exception("Push application \"{$type}\" not yet implemented");
      }
    }


    public function push($id, $tag) {

      global $conf;

      foreach ($conf['deploy']['target']['hosts'] as $host) {

        $src = "../deploy/tags/{$tag}";
        $dst = "{$host['name']}:{$host['path']}/{$tag}";

        $cmd = $this->_getCommand($host, $src, $dst);

        drupal_set_message("Pushing data from {$src} to {$dst}");
        db_query("
          INSERT INTO {deploy_push} (
            start,
            command
          ) VALUES (
            NOW(),
            '%s'
          )
        ", $cmd);
        $pid = db_last_insert_id('deploy_push','id');

        db_query("
          INSERT INTO {deploy_deploy_push} (
            deploy_id,
            push_id
          ) VALUES (
            $id,
            $pid
          )
        ");
        db_query("
          UPDATE
            {deploy_push}
          SET
            stop = NOW(),
            output = '%s'
          WHERE
            id = {$pid}
        ", `$cmd`); // Actual command execution happens here.

      }

    }

    protected function _getCommand($host, $src, $dst) {
      throw new Exception('Not implimented');
    }

  }



  class sourcecontrol_Push_Rsync extends sourcecontrol_Push {

    protected function _getCommand($host, $src, $dst) {

      global $conf;

      $options  = " -av --delete --exclude=.svn --exclude=.git* --exclude=CVS --exclude=.bzr --exclude=.hg*";
      $options .= " {$conf['deploy']['push']['args']}";
      $options .= " -e \"ssh -o LogLevel=ERROR -o StrictHostKeyChecking=no -o UserKnownHostsFile=../deploy/.ssh/known_hosts -l {$host['user']} -p {$host['port']} -i ../deploy/.ssh/keys/{$host['key']}\"";

      return "{$this->binary} {$options} {$src}/ {$dst}/ 2>&1";

    }

  }
