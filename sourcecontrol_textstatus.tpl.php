<?php


  class sourcecontrol_Pretty {

    private $_colours;
    private $_cols;

    public function __construct($columns='auto', $colour=TRUE) {
      $this->_setupcols($columns);
      $this->_setupcolours();
      $this->_setuptypes($colour);
    }


    public function timeify($start, $stop) {
      return sprintf("%0.02f", (strtotime($stop) - strtotime($start)) / 60);
    }


    public function wrapify($s, $type, $indent=0) {

      $lines = split("\n", wordwrap($s, $this->_cols - $indent - 1, "\n", TRUE));

      $output = ($indent) ? "\n" : '';
      foreach ($lines as $line) {
        if ($line) {
          $output .= sprintf(
            "%s%s%s%s\n",
            str_repeat(" ", $indent),
            $this->_types->$type,
            $line,
            $this->_colours->none
          );
        }
      }

      return $output;

    }


    private function _setupcols($columns) {

      if ($columns == 'auto') {

        $this->_cols = 80;
        if (isset($_ENV['COLUMNS']) && is_numeric($_ENV['COLUMNS']) && $_ENV['COLUMNS'] > 0) {
          $this->_cols = ($_ENV['COLUMNS'] > 150) ? 150 : (int) $_ENV['COLUMNS'];
        }

      }
      else {

        $this->_cols = $columns;

      }

    }


    private function _setupcolours() {
      $this->_colours = (object) array(
        'black'       => "\033[0;30m",
        'blue'        => "\033[0;34m",
        'blue_light'  => "\033[1;34m",
        'green'       => "\033[0;32m",
        'green_light' => "\033[1;32m",
        'cyan'        => "\033[0;36m",
        'cyan_light'  => "\033[1;36m",
        'red'         => "\033[0;31m",
        'red_light'   => "\033[1;31m",
        'purple'      => "\033[0;35m",
        'brown'       => "\033[0;33m",
        'grey'        => "\033[1;30m",
        'grey_light'  => "\033[0;37m",
        'pink'        => "\033[1;35m",
        'yellow'      => "\033[1;33m",
        'white'       => "\033[1;37m",
        'none'        => "\033[0m",
      );
    }


    private function _setuptypes($colourise) {

      $this->_types = (object) array(
        'command' => '',
        'output'  => '',
        'time'    => '',
      );

      if ($colourise) {
        $this->_types = (object) array(
          'command' => $this->_colours->brown,
          'output'  => $this->_colours->green_light,
          'time'    => $this->_colours->cyan,
        );
      }

    }

  }

  $pretty = new sourcecontrol_Pretty($columns, $colour);

  if ($status):

?>
Tag: <?php print $pretty->wrapify($status->tag, 'time'); ?>

Started: <?php print $pretty->wrapify($status->start, 'time'); ?>

Code Checkout
  Command: <?php print $pretty->wrapify($status->checkout->command, 'command', 4); ?>
  Output: <?php print $pretty->wrapify($status->checkout->output, 'output', 4); ?>
  Time: <?php print $pretty->wrapify($pretty->timeify($status->checkout->start, $status->checkout->stop) .'minutes', 'time', 4); ?>

Backup Database
  Command: <?php print $pretty->wrapify($status->backup_db->command, 'command', 4); ?>
  Output: <?php print $pretty->wrapify($status->backup_db->output, 'output', 4); ?>
  Time: <?php print $pretty->wrapify($pretty->timeify($status->backup_db->start, $status->backup_db->stop) .'minutes', 'time', 4); ?>

Backup Database
  Command: <?php print $pretty->wrapify($status->backup_ugc->command, 'command', 4); ?>
  Output: <?php print $pretty->wrapify($status->backup_ugc->output, 'output', 4); ?>
  Time: <?php print $pretty->wrapify($pretty->timeify($status->backup_ugc->start, $status->backup_ugc->stop) .'minutes', 'time', 4); ?>

Remote Command(s)
<?php foreach ($status->precommand as $s): ?>
  Command: <?php print $pretty->wrapify($s->command, 'command', 4); ?>
  Output: <?php print $pretty->wrapify($s->output, 'output', 4); ?>
  Time: <?php print $pretty->wrapify($pretty->timeify($s->start, $s->stop) .'minutes', 'time', 4); ?>
  <? endforeach ?>

Code Push
<?php foreach ($status->push as $s): ?>
  Command: <?php print $pretty->wrapify($s->command, 'command', 4); ?>
  Output:  <?php print $pretty->wrapify($s->output, 'output', 4); ?>
  Time:    <?php print $pretty->wrapify($pretty->timeify($s->start, $s->stop) .'minutes', 'time', 4); ?>
  <? endforeach ?>

Remote Command(s)
<?php foreach ($status->postcommand as $s): ?>
  Command: <?php print $pretty->wrapify($s->command, 'command', 4); ?>
  Output:  <?php print $pretty->wrapify($s->output, 'output', 4); ?>
  Time:    <?php print $pretty->wrapify($pretty->timeify($s->start, $s->stop) .'minutes', 'time', 4); ?>
  <? endforeach ?>

Switch Release
<?php foreach ($status->switchrelease as $s): ?>
  Command: <?php print $pretty->wrapify($s->command, 'command', 4); ?>
  Output:  <?php print $pretty->wrapify($s->output, 'output', 4); ?>
  Time:    <?php print $pretty->wrapify($pretty->timeify($s->start, $s->stop) .'minutes', 'time', 4); ?>
  <? endforeach ?>

Update
  Command: <?php print $pretty->wrapify($status->update->command, 'command', 4); ?>
  Output:  <?php print $pretty->wrapify($status->update->output, 'output', 4); ?>
  Time:    <?php print $pretty->wrapify($pretty->timeify($status->update->start, $status->update->stop) .'minutes', 'time', 4); ?>

Stopped:    <?php print $pretty->wrapify($status->stop, 'time'); ?>
Total time: <?php print $pretty->wrapify($pretty->timeify($status->start, $status->stop) .'minutes', 'time'); ?>

<?php
  else:
    print "No log for the requested plan id";
  endif
?>