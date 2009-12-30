<?php

// $Id$

/**
 * @file
 *  Taxonomy Menu functions that access the db
 */

/**
 * Get an array of the tid's from the parent
 *
 * @param $tid
 * @return array of tid
 */
function _taxonomy_menu_get_parents($tid) {
  $output = array();
  $result = taxonomy_get_parents($tid);
  foreach ($result as $key => $item) {
    $output[] = $key;
  }
  return $output;
}

/**
 * Get all of the tid for a given vid
 *
 * @param $vid
 * @return array of $tid
 */
function _taxonomy_menu_get_terms($vid) {
  $output = array();
  $result = db_query(db_rewrite_sql('SELECT t.tid FROM {term_data} t WHERE t.vid = %d','t', 'tid'), $vid);
  while ($data = db_fetch_object($result)) {
    $output[] = $data->tid;
  }
  return $output;
}

/**
 * used to get the count without children
 *
 * @param $tid
 */
function _taxonomy_menu_term_count($tid) {
  return db_result(db_query(db_rewrite_sql('SELECT COUNT(n.nid) AS c FROM {term_node} t INNER JOIN {node} n ON t.vid = n.vid WHERE n.status = 1 AND t.tid = %d'), $tid));
}

/**
 * get the name of a vocabulary
 *
 * @param $vid
 * @return vocab name
 */
function _taxonomy_menu_get_vocab_name($vid) {
  return db_result(db_query('SELECT name FROM {vocabulary} WHERE vid = %d', $vid));
}

/**
 * get the name of a term
 * @param $tid
 * @return term name
 */
function _taxonomy_menu_get_term_name($tid) {
  return db_result(db_query(db_rewrite_sql('SELECT t.name FROM {term_data} t WHERE tid = %d','t', 'tid'), $tid));
}

/**
 * get a list of the vocab names
 *
 * @return array of vocab objects
 */
function _taxonomy_menu_list_vocabs() {
  $output = array();
  $result = db_query('SELECT vid, name FROM {vocabulary}');
  while ($data = db_fetch_object($result)) {
    $output[] = $data;
  }
  return $output;
}

/**
 * return the value of an option
 *
 * @param $name
 * @param $type
 * @return value of the option
 */
function taxonomy_menu_get_option($name, $type, $default, $type_key = '') {
  $option =  db_result(db_query("SELECT value FROM {taxonomy_menu_options} WHERE name = '%s' AND type = '%s' and type_key = '%s'", $name, $type, $type_key));
  return isset($option) ? $option : $default;
}

function _taxonomy_menu_get_options_by_key($type, $type_key) {
  $output = array();
  $type_key = strval($type_key);
  $result = db_query("SELECT * FROM {taxonomy_menu_options} WHERE type = '%s' and type_key = '%s'", $type, $type_key);
  while ($data = db_fetch_object($result)) {
    $output[$data->name] = $data;
  }
  return $output;
}
/**
 * get a list of the options
 *
 * @param @type
 *
 * @return array of option objects
 */
function taxonomy_menu_list_options($type) {
  $output = array();
  $result = db_query("SELECT * FROM {taxonomy_menu_options} WHERE type = '%s'", $type);
  while ($data = db_fetch_object($result)) {
    $output[] = $data;
  }
  return $output;
}

/**
 * Save the taxonomy options.  Check to see if the option exists first
 * @param $option object
 *  ->name
 *  ->type
 *  ->value
 *  ->type_key
 */
function taxonomy_menu_save_option($option) {
  $old_value = taxonomy_menu_get_option($option->name, $option->type, NULL, $option->type_key);
  if (!isset($old_value) || $old_value === FALSE) {
    //option does not exist, insert new record
    drupal_write_record('taxonomy_menu_options', $option);
  }
  else {
    //record exists and update it
    drupal_write_record('taxonomy_menu_options', $option, array('name', 'type', 'type_key'));
  }
}

/**
 * Get vid, tid for a given mlid
 *
 * @param $mlid
 * @return array of vid, tid
 */
function _taxonomy_menu_get_item($mlid) {
  return db_fetch_array(db_query('SELECT tid, vid FROM {taxonomy_menu} WHERE mlid = %d', $mlid));
}

/**
 * Get a list of Term Sets that are not assicated with the menu group.
 * @param $mgid
 * @return array
 */
function taxonomy_menu_get_avaialbe_term_sets($mgid) {
  $output = array();
  $result = db_query('SELECT ts.tsid, ts.name FROM {taxonomy_menu_group_term_set} AS tsg
    LEFT JOIN {taxonomy_menu_term_set} AS ts
    ON tsg.tsid = ts.tsid
    WHERE tsg.mgid = %d', $mgid);

  while ($data = db_fetch_object($result)) {
    $output[$data->tsid] = t($data->name);
  }

  return $output;
}

/**
 * Function to pull the items for a menu group
 */
function taxonomy_menu_get_group_items($mgid) {
  $result = db_fetch_object(db_query('SELECT items FROM {taxonomy_menu_group} WHERE mgid = %d', $mgid));
  return unserialize($result);
}

/**
 * Flattens Array
 */
function taxonomy_menu_flatten_array($array) {
  foreach ($a as $k=>$v) {
    $a[$k]=(array)$v;
  }
  return call_user_func_array(array_merge,$a);
}
