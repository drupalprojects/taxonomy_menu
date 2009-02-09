<?php
// $Id$

/**
 * @file taxonomy_menu_database.inc
 *
 * The functions that interfact with the DB layer
 */

/**
 * Function to interface with Database
 * @param string $type
 *  Options:
 *    TAXONOMY_MENU_INSERT:  insert data into the database
 *    TAXONOMY_MENU_SELECT:  return data from the database
 *    TAXONOMY_MENU_DELETE:  remove data from the database
 * @param $args
 *  Array('filed'  => 'value)
 *    ex:  array('tid' => '3')
 * @param $options  ALL ELEMENTS ARE OPTIONAL
 *  array(
 *    'return'        => array('field1', field2') -- used for select statements
 *    'delete_all     => BINARY (TRUE if yes, FALSE if no)  used in menu rebuild
 *    'return_type'   => int (TAXONOMY_MENU_VALUE/TAXONOMY_MENU_OBJECT(returns and array of objects)/TAXONOMY_MENU_ARRAY)
 *    'return_array' => array('key' => 'key field', 'value' => 'value field')  ex: (array('key' => 'tid', 'value' => 'mlid')
 *  )
 */
function taxonomy_menu_database($type, $args = array(), $options = array()) {

  if ($options['delete_all']) {
    //detete all the records from {taxonomy_menu and mlids from menu_links
    //pass in the $vid as the arg
    $menu_terms = _taxonomy_menu_get_menu_terms($args['vid']);
    if (!empty($menu_terms)) {
      foreach ($menu_terms as $tid => $mlid) {
        db_query('DELETE FROM {menu_links} WHERE mlid = %d', $mlid);
      }
      db_query('DELETE FROM {taxonomy_menu} WHERE vid = %d', $vid);
    }
  }
  else {
    //BUILD SQL STATEMENT
    switch ($type) {
      case TAXONOMY_MENU_SELECT:
        $sqlStart = sprintf("SELECT %s FROM {taxonomy_menu} WHERE ", implode(", ", $options['return']));
        $sqlWhere = _taxonomy_menu_database_get_statement($args);
        $sql = $sqlStart . $sqlWhere;
        break;

      case TAXONOMY_MENU_INSERT:
        $sql = 'INSERT INTO {taxonomy_menu} ' . _taxonomy_menu_database_get_insert($args);
        break;
    }

    //RUN SQL STATEEMNT
    $result = db_query($sql);
    if (!empty($options[return_type])) {
      switch ($options[return_type]) {
        case TAXONOMY_MENU_VALUE:
          $output = db_result($result);
          break;

        case TAXONOMY_MENU_ARRAY:
          $output = array();
          $key = $options['return_array']['key'];
          $value = $options['return_array']['value'];

          while ($data = db_fetch_array($result)) {
            $output[$data[$key]] = $data[$value];
          }
          break;

        case TAXONOMY_MENU_OBJECT:
          $output = array();
          //return an array of objects
          while ($data = db_fetch_object($result)) {
            $output[] .= $data;
          }
      }
      return $output;
    }

  }
}

/**
 *  helper function to create the insert statement
 *
 * @param array $args
 * @return string
 */
function _taxonomy_menu_database_get_insert($args = array()) {
  $keys = implode(", ", array_keys($args));
  $values = implode(", ", array_values($args));

  $output = sprintf('(%s) VALUES (%s)', $keys, $values);

  return $output;
}

/**
 * helper function to build sql statement
 * @param int (TAXONOMY_MENU_INSERT/TAXONOMY_MENU_SELECT
 * @param array $args
 * @return string
 */
function _taxonomy_menu_database_get_statement($args = array()) {
  $limit = count($args);
  $i = 0;

  foreach ($args as $key => $value) {
    $output .= sprintf('%s = %d', $key, $value);
    if ($i <$limit) {
      $output .= ' AND ';
    }
  }

  return $output;
}