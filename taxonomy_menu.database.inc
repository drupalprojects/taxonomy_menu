<?php

// $Id$

/**
 * @author Neil Hastings      <http://drupal.org/user/245817>
 * @author Mark Theunissen    <http://drupal.org/user/108606>
 * @author Afief Halumi       <http://drupal.org/user/237472>
 * @file taxonomy_menu.database.inc
 * Database functions
 **/


 /**
 *  helper function to insert a menu item
 *
 * @param $mlid
 * @param $tid
 * @param $vid
 */
function _taxonomy_menu_insert_menu_item($mlid, $tid, $vid) {
  db_query('INSERT INTO {taxonomy_menu} (mlid, tid, vid) VALUES (%d, %d, %d)', $mlid, $tid, $vid);
}

/**
 * Return the corresponding menu link id.
 *
 * @param $tid
 *   the term's id
 */
function _taxonomy_menu_get_mlid($tid) {
  return db_result(db_query('SELECT mlid FROM {taxonomy_menu} WHERE tid = %d', $tid));
}

/**
 * Retrieve the term / menu relations for a vocab.
 *
 * @param $vid
 *   vocabulary's id
 * @return
 *   array(tid => mlid)
 */
function _taxonomy_menu_get_menu_terms($vid) {

  $result = db_query('SELECT mlid, tid FROM {taxonomy_menu} WHERE vid = %d', $vid);
  $menu_terms = array();
  while ($data = db_fetch_object($result)) {
    $menu_terms[$data->tid] = $data->mlid;
  }
  return $menu_terms;
}

 /**
	  * Delete all links associated with this vocab from both the taxonomy_menu
	  * table and the menu_link table.
	  *
	  * @param $vid
	  *   vocabulary's id
	  */
	 function _taxonomy_menu_delete_all($vid) {
	   $menu_terms = _taxonomy_menu_get_menu_terms($vid);
	   if (!empty($menu_terms)) {
	     foreach ($menu_terms as $tid => $mlid) {
	       db_query('DELETE FROM {menu_links} WHERE mlid = %d', $mlid);
	     }
	     db_query('DELETE FROM {taxonomy_menu} WHERE vid = %d', $vid);
	   }
	 }