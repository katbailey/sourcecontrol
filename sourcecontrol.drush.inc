<?php

  /**
   *
   * Our very own drush-command(s).  If you're using this a starting point for
   * your own drush stuff, note that the file name is relevant.  You *have* to
   * name it <modulename>.drush.inc or it just magically doesn't work and
   * doesn't tell you why.
   *
   */


  /**
   *
   * The "menu-hook" for drush stuff.  Specify your command(s) here and include
   * other useful bits like arguments and options.
   *
   * @see drush_parse_command() for a list of recognized keys.
   *
   */
  function sourcecontrol_drush_command() {
    return array(
      'push' => array(
        'description' => "Pushes the source code to the next server in the chain",
        'arguments' => array(
          'tag' => 'The tag you wish to push to the target',
        ),
        'options' => array(
          'target' => 'Either "next" or "self".  The default is "next"',
        ),
        'examples' => array(
          'sudo -u <webserveruser> drush push hotwheels-0.0.9',
          'sudo -u <webserveruser> drush push hotwheels-0.0.9 --target=next',
          'sudo -u <webserveruser> drush push hotwheels-0.0.9 --target=self',
        ),
      ),
      'push-log' => array(
        'description' => 'Returns a complete log of what went down on the last push',
        'examples' => array(
          'drush push-log'
        ),
      ),
    );
  }


  /**
   *
   * Help.  Note that all of the switch cases have to be keyed with "drush:"
   *
   */
  function sourcecontrol_drush_help($section) {
    switch ($section) {
      default:
        return dt("I don't understand why we need to describe commands twice.");
    }
  }


  /* Methods
  ------------------------------------------------------------------------- */

  /**
   *
   * We check the target and tags here so we don't have to do it in the actual
   * action function.
   *
   */
  function drush_sourcecontrol_push_validate($tag=NULL) {

    require_once drupal_get_path('module', 'sourcecontrol') .'/sourcecontrol_sc.inc';

    $target = drush_get_option('target', 'next');
    if (!in_array($target, array('next','self'))) {
      return drush_set_error('INVALID_TARGET', dt('Invalid target'));
    }

    $sc = _sourcecontrol_get_sc();
    if (!in_array($tag, $sc->gettags())) {
      return drush_set_error('INVALID_TAG', dt('Invalid tag'));
    }

  }


  /**
   *
   * The actual work
   *
   */
  function drush_sourcecontrol_push($tag) {
    sourcecontrol_push(
      drush_get_option('target', 'next'),
      $tag
    );
  }


  /**
   *
   * Fetches the list of stuff that happened in the last run
   *
   */
  function drush_sourcecontrol_push_log() {
    print theme('sourcecontrol_textstatus', 'auto', TRUE, sourcecontrol_fetchlog());
  }
