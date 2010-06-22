<?php

  class sourcecontrol_Log {

    public $open;

    public function __construct() {
      $this->open = FALSE;
    }


    public function start($tag, $plan) {

      global $conf;

      db_query("
        INSERT INTO {deploy} (
          plan_id,
          tag,
          start
        ) VALUES (
          %d,
          '%s',
          NOW()
        )
      ", $plan, $tag);

      $this->id = db_last_insert_id('deploy','id');
      $this->open = TRUE;

      return $this->id;

    }


    public function stop($error="") {
      db_query("
        UPDATE
          deploy
        SET
          error = '%s',
          stop = NOW()
        WHERE
          id = {$this->id}
      ", $error);
      $this->open = FALSE;
    }

  }
