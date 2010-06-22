<?php

  class sourcecontrol_Command {

    /**
     *
     * Executes an arbitrary string on the target server(s) (Crazy!)
     *
     */
    public function arbitrary($id, $target, $type) {

      if (isset($target['commands']) && isset($target['commands'][$type]) && $target['commands'][$type]) {

        $base_cmd = "cd active/; {$target['commands'][$type]}";
        foreach ($target['hosts'] as $host) {

          drupal_set_message("Remote-command sent to {$host['name']}: {$base_cmd}");

          $cmd = self::wrapcommand($host, $base_cmd);

          db_query("INSERT INTO {deploy_{$type}command} (start, command) VALUES (NOW(), '%s')", $cmd);
          $pid = db_last_insert_id("deploy_{$type}command", 'id');

          db_query("INSERT INTO {deploy_deploy_{$type}command} (deploy_id, {$type}command_id) VALUES ($id,$pid)");

          // Actual command execution happens here.
          db_query("UPDATE {deploy_{$type}command} SET stop = NOW(), output = '%s' WHERE id = {$pid}", `$cmd`);

        }

      }

    }


    /**
     *
     * I suppose that this could live in the module itself, but since it's done
     * via a remote command, it just seemed logical to have it here.
     *
     */
    public function switchrelease($id, $tag, $hosts) {

      foreach ($hosts as $host) {

        drupal_set_message("Switching the release on {$host['name']} to {$tag}");

        $cmd = self::wrapcommand($host, "rm active; ln -s $tag active");

        db_query("
          INSERT INTO {deploy_switchrelease} (
            start,
            command
          ) VALUES (
            NOW(),
            '%s'
          )
        ", $cmd);
        $pid = db_last_insert_id("deploy_switchrelease",'id');

        db_query("
          INSERT INTO {deploy_deploy_switchrelease} (
            deploy_id,
            switchrelease_id
          ) VALUES (
            $id,
            $pid
          )
        ");

        // Actual command execution happens here.
        db_query("
          UPDATE
            {deploy_switchrelease}
          SET
            stop = NOW(),
            output = '%s'
          WHERE
            id = {$pid}
        ", `$cmd`);

      }

    }


    /**
     *
     * Same line of thinking here as in switchrelease().  This too is a remote
     * command, so it'll live here until someone insists otherwise.
     *
     */
    public function update($id, $host, $name) {

      $cmd = self::wrapcommand($host, "cd active/htdocs/ && COLUMNS=80 drush --yes --uri=http://{$name} updatedb");

      db_query("
        INSERT INTO {deploy_update} (
          start,
          command
        ) VALUES (
          NOW(),
          '%s'
        )
      ", $cmd);
      $pid = db_last_insert_id("deploy_update",'id');

      db_query("
        UPDATE
          {deploy}
        SET
          update_id = {$pid}
        WHERE
          id = {$id}");

      // Actual command execution happens here.
      db_query("
        UPDATE
          {deploy_update}
        SET
          stop = NOW(),
          output = '%s'
        WHERE
          id = {$pid}
        ", `$cmd`);

    }


    /**
     *
     * What do you think this does?
     *
     */
    public function backupdatabase($id) {

      global $conf;

      $type = $conf['deploy']['target']['backup']['db']['type'];
      switch ($type) {
        case 'mysql':
          $cmd = sprintf(
            "mysqldump --add-drop-table -u%s %s %s %s > %s",
            escapeshellarg($conf['deploy']['target']['backup']['db']['user']),
            ($conf['deploy']['target']['backup']['db']['pass']) ? '-p'. escapeshellarg($conf['deploy']['target']['backup']['db']['pass']) : '',
            ($conf['deploy']['target']['backup']['db']['host']) ? '-h'. escapeshellarg($conf['deploy']['target']['backup']['db']['host']) : '',
            escapeshellarg($conf['deploy']['target']['backup']['db']['name']),
            escapeshellarg("/tmp/{$conf['deploy']['target']['backup']['db']['name']}.sql")
          );
          break;
        default:
          throw new Exception("Database type \"{$type}\" is not yet implemented");
      }

      $cmd = self::wrapcommand($conf['deploy']['target']['hosts'][0], $cmd);

      drupal_set_message("Backing up Database:<br />&nbsp;&nbsp;{$cmd}");

      db_query("
        INSERT INTO {deploy_backup_db} (
          start,
          command
        ) VALUES (
          NOW(),
          '%s'
        )
      ", $cmd);
      $pid = db_last_insert_id('deploy_backup_db','id');

      db_query("
        UPDATE
          {deploy}
        SET
          backup_db_id = {$pid}
        WHERE
          id = {$id}
      ");

      db_query("
        UPDATE
          {deploy_backup_db}
        SET
          stop = NOW(),
          output = '%s'
        WHERE
          id = {$pid}
      ", `$cmd`); // Actual command execution happens here.

    }


    /**
     *
     * Backup UGC from one of the hosts.  Note that we're making the assumption
     * that the UGC is global across n servers, so we opt to run this against
     * only one of them, the first one to be exact.
     *
     */
    public function backupugc($id) {

      global $conf;

      $u = (object) $conf['deploy']['target']['backup']['ugc']; // Aliasing the ugc config
      $cmd = self::wrapcommand(
        $conf['deploy']['target']['hosts'][0], // First server only
        "mkdir -p '{$u->dst}' && rsync -av {$u->src}/ {$u->dst}/"
      );

      drupal_set_message("Backing up UGC:<br />&nbsp;&nbsp;{$cmd}");

      db_query("
        INSERT INTO {deploy_backup_ugc} (
          start,
          command
        ) VALUES (
          NOW(),
          '%s'
        )
      ", $cmd);
      $pid = db_last_insert_id('deploy_backup_ugc','id');

      db_query("
        UPDATE
          {deploy}
        SET
          backup_ugc_id = {$pid}
        WHERE
          id = {$id}
      ");

      db_query("
        UPDATE
          {deploy_backup_ugc}
        SET
          stop = NOW(),
          output = '%s'
        WHERE
          id = {$pid}
      ", `$cmd`); // Actual command execution happens here.

    }


    /**
     *
     * All remote commands need to be issued in a specific fashion including
     * various options passed to SSH and escaping the input etc.
     *
     */
    public static function wrapcommand($host, $cmd) {

      $cmd = "cd {$host['path']}; {$cmd}"; // Start somewhere logical so we're not filling our command with a bunch of concrete paths

      return "ssh -o LogLevel=ERROR -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -l {$host['user']} -p {$host['port']} -i ../deploy/.ssh/keys/{$host['key']} {$host['name']} ". escapeshellarg($cmd) ." 2>&1";

    }

  }
