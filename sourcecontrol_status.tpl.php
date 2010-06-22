<?php
  /**
   *
   * Available variables:
   *   @param  $status  A single recordset from the deploy log.
   *
   */

  if ($status) {

    // Persistent messages
    print '<div id="persistentmessages">';
    foreach (array('status','error') as $t) {
      if ($_SESSION['persistentmessages'][$t]) {
        print "<div class=\"messages {$t}\"><ul>";
        foreach ($_SESSION['persistentmessages'][$t] as $m) {
          print "<li>{$m}</li>";
        }
        print '</ul></div>';
      }
    }
    print '</div>';

    print "
      <h1>Tag</h1>
      <p>{$status->tag}</p>

      <h1>Started</h1>
      <p>{$status->start}</p>
    ";

    if ($status->checkout->start) {
      print "
        <h1>Code Checkout</h1>
        <dl>
          <dt>Command (<span class=\"time\">{$status->checkout->start}</span>)</dt>
          <dd class=\"command\">{$status->checkout->command}</dd>
          <dt>Output (<span class=\"time\">{$status->checkout->stop}</span>)</dt>
          <dd class=\"output\">{$status->checkout->output}</dd>
        </dl>
      ";
    }

    if ($status->backup_db->start) {
      print "
        <h1>Backup Database</h1>
        <dl>
          <dt>Command (<span class=\"time\">{$status->backup_db->start}</span>)</dt>
          <dd class=\"command\">{$status->backup_db->command}</dd>
          <dt>Output (<span class=\"time\">{$status->backup_db->stop}</span>)</dt>
          <dd class=\"output\" >{$status->backup_db->output}</dd>
        </dl>
      ";
    }

    if ($status->backup_ugc->start) {
      print "
        <h1>Backup UGC</h1>
        <dl>
          <dt>Command (<span class=\"time\">{$status->backup_ugc->start}</span>)</dt>
          <dd class=\"command\">{$status->backup_ugc->command}</dd>
          <dt>Output (<span class=\"time\">{$status->backup_ugc->stop}</span>)</dt>
          <dd class=\"output\">{$status->backup_ugc->output}</dd>
        </dl>
      ";
    }

    if ($status->precommand) {

      print "
        <h1>Remote Command(s)</h1>
        <dl>
      ";

      foreach ($status->precommand as $s) {
        print "
          <dt>Command (<span class=\"time\">{$s->start}</span>)</dt>
          <dd class=\"command\">{$s->command}</dd>
          <dt>Output (<span class=\"time\">{$s->stop}</span>)</dt>
          <dd class=\"output\">{$s->output}</dd>
        ";
      }

      print "
        </dl>
      ";

    }

    if ($status->push) {

      print "
        <h1>Code Push</h1>
        <dl>
      ";

      foreach ($status->push as $s) {
        print "
          <dt>Command (<span class=\"time\">{$s->start}</span>)</dt>
          <dd class=\"command\">{$s->command}</dd>
          <dt>Output (<span class=\"time\">{$s->stop}</span>)</dt>
          <dd class=\"output\">{$s->output}</dd>
        ";
      }

      print "
        </dl>
      ";

    }

    if ($status->postcommand) {

      print "
        <h1>Remote Command(s)</h1>
        <dl>
      ";

      foreach ($status->postcommand as $s) {
        print "
          <dt>Command (<span class=\"time\">{$s->start}</span>)</dt>
          <dd class=\"command\">{$s->command}</dd>
          <dt>Output (<span class=\"time\">{$s->stop}</span>)</dt>
          <dd class=\"output\">{$s->output}</dd>
        ";
      }

      print "
        </dl>
      ";

    }

    if ($status->switchrelease) {

      print "
        <h1>Switch Release</h1>
        <dl>
      ";

      foreach ($status->switchrelease as $s) {
        print "
          <dt>Command (<span class=\"time\">{$s->start}</span>)</dt>
          <dd class=\"command\">{$s->command}</dd>
          <dt>Output (<span class=\"time\">{$s->stop}</span>)</dt>
          <dd class=\"output\">{$s->output}</dd>
        ";
      }

      print "
        </dl>
      ";

    }

    if ($status->update->start) {
      print "
        <h1>Update</h1>
        <dl>
          <dt>Command (<span class=\"time\">{$status->update->start}</span>)</dt>
          <dd class=\"command\">{$status->update->command}</dd>
          <dt>Output (<span class=\"time\">{$status->update->stop}</span>)</dt>
          <dd class=\"output\">{$status->update->output}</dd>
        </dl>
      ";
    }

    if ($status->stop) {
      print "
        <h1>Stopped</h1>
        <p>{$status->stop}</p>
      ";
    }

  }
