<?php

  class sourcecontrol_SourceControl {

    protected $binary;
    protected $cmd;
    protected $src;
    protected $dst;

    private $_tags;

    public function __construct() {

      global $conf;

      $bin = $conf['deploy']['sourcecontrol']['type'];
      $this->binary = trim(`which {$bin}`);
      $this->_tags  = array();

      if (!$this->binary)
        throw new Exception("Cannot locate source control binary \"{$bin}\"");

    }


    public static function factory($type) {
      switch ($type) {
        case 'svn':
          return new sourcecontrol_SourceControl_Subversion();
        case 'cvs':
        case 'git':
        case 'bazaar':
        case 'hg':
        default:
          throw new Exception("Source control application \"{$type}\" not yet implemented");
      }
    }

    /**
     *
     * Updates the local checkout that is this site (not the one(s) in
     * deploy/tags) and does so by connecting to this box (self) over SSH.
     *
     */
    public function updatelocal() {
      throw new Exception('Not yet implemented');
    }


    /**
     *
     * Performs the source checkout, getting the details of how to do that from
     * the subclass via _setCommand().
     *
     */
    public function checkout($id, $tag) {

      global $conf; // This makes me cry a little

      $this->src = "{$conf['deploy']['sourcecontrol']['location']}/tags/{$tag}";
      $this->dst = "../deploy/tags/{$tag}";

      $this->_setCommand();

      drupal_set_message("Checking out content from {$this->src} to {$this->dst}");

      db_query("
        INSERT INTO {deploy_checkout} (
          start,
          command
        ) VALUES (
          NOW(),
          '%s'
        )
      ", $this->cmd);
      $pid = db_last_insert_id('deploy_checkout','id');

      db_query("
        UPDATE
          {deploy}
        SET
          checkout_id = {$pid}
        WHERE
          id = {$id}
      ");

      db_query("
        UPDATE
          {deploy_checkout}
        SET
          stop = NOW(),
          output = '%s'
        WHERE
          id = {$pid}
      ", `$this->cmd`); // Actual command execution happens here.

    }


    // Protected Methods -----------------------------------------------------

    /**
     *
     * A recursive check on a directory for write access.
     *
     */
    protected function _is_writable($d) {

      $dh = opendir($d);
      while ($f = readdir($dh)) {
        if ($f != '.' && $f != '..' && $f != '.svn' && (!is_writable("{$d}/{$f}") || (is_dir("{$d}/{$f}") && !$this->_is_writable("{$d}/{$f}")))) {
          closedir($dh);
          return FALSE;
        }
      }
      closedir($dh);

      return TRUE;

    }


    /**
     *
     * Grabs a list of tags available to push.
     *
     */
    function gettags() {
      return array();
    }

  }



  class sourcecontrol_SourceControl_Subversion extends sourcecontrol_SourceControl {

    public function __construct() {

      global $conf;

      parent::__construct();

      $d = realpath('../deploy/.subversion');
      if (!file_exists($d) || !is_dir($d))
        throw new Exception("Cannot locate .subversion directory: {$d}");

      if (!$dh = opendir($d))
        throw new Exception("Cannot open .subversion directory: {$d}");

      if (!$this->_is_writable($d))
        throw new Exception("Cannot write to .subversion directory: {$d}");

      // Rewrite this.binary to include all of the common options.
      $this->binary = "{$this->binary} --config-dir {$d} {$conf['deploy']['sourcecontrol']['args']}";

    }


    /**
     *
     * @see parent.gettags()
     *
     */
    function gettags() {

      // No need to ask the server twice.
      if ($this->_tags) {
        return $this->_tags;
      }

      global $conf;

      $cmd = "{$this->binary} list '{$conf['deploy']['sourcecontrol']['location']}/tags/' 2>&1";

      foreach (split("\n", `$cmd`) as $tag) {
        if ($tag) {
          $tag = preg_replace('/\/$/', '', $tag);
          $this->_tags[$tag] = $tag;
        }
      }

      return $this->_tags;

    }


    /**
     *
     * @see parent::updatelocal();
     *
     */
    public function updatelocal($id, $host, $tag) {

      $cmd = sourcecontrol_Command::wrapcommand($host, "cd $tag && {$this->binary} update 2>&1"); // Run svn update against all tags

      db_query("
        INSERT INTO {deploy_checkout} (
          start,
          command
        ) VALUES (
          NOW(),
          '%s'
        )
      ", $cmd);
      $pid = db_last_insert_id('deploy_checkout','id');

      db_query("
        UPDATE
          {deploy}
        SET
          checkout_id = {$pid}
        WHERE
          id = {$id}
      ");

      db_query("
        UPDATE
          {deploy_checkout}
        SET
          stop = NOW(),
          output = '%s'
        WHERE
          id = {$pid}
      ", `$cmd`); // Actual command execution happens here.

    }


    public function _setCommand() {
      $this->cmd = "{$this->binary} checkout '{$this->src}' '{$this->dst}' 2>&1";
    }

  }
