<?php
// $Id$

/*
 * Implementation of hook_pathauto() for taxonomy module
 */
function taxonomy_menu_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'taxonomy_menu';
      $settings['token_type'] = 'taxonomy_menu';
      $settings['groupheader'] = t('Taxonomy Menu path settings');
      $settings['patterndescr'] = t('Default path pattern (applies to all vocabularies with blank patterns below)');
      $settings['patterndefault'] = '[tax-menu]/[vocab-raw]/[cat-raw]';
      $patterns = token_get_list('taxonomy_menu');
      foreach ($patterns as $type => $pattern_set) {
        if ($type != 'global') {
          foreach ($pattern_set as $pattern => $description) {
            $settings['placeholders']['['. $pattern .']'] = $description;
          }
        }
      }
      $settings['bulkname'] = t('Bulk generate aliases for Taxonomy Menu items');
      $settings['bulkdescr'] = t('Generate aliases for all existing Taxonomy Menu items.');

      $vocabularies = taxonomy_get_vocabularies();
      if (sizeof($vocabularies) > 0) {
        $settings['patternitems'] = array();
        $forum_vid = variable_get('forum_nav_vocabulary', '');
        foreach ($vocabularies as $vocab) {
          if ($vocab->vid != $forum_vid) {
            $vocabname = $vocab->name;
            $fieldlabel = t('Pattern for all %vocab-name paths', array('%vocab-name' => $vocabname));
            $settings['patternitems'][$vocab->vid] = $fieldlabel;
          }
        }
      }
      return (object) $settings;

    default:
      break;
  }
}

/**
 * Generate aliases for all categories
 */ 
function taxonomy_menu_pathauto_bulkupdate() {
  $count = 0;
  foreach (taxonomy_get_vocabularies() as $category) { 
    // The Base Vocabulary path
    $path =  variable_get('taxonomy_menu_display_page', 'category') .'/'. $category->vid;
  
    $tree = taxonomy_get_tree($category->vid);
    $old_depth = -1;
    $old_path = $path;
  
    foreach ($tree as $term) {
      // get the tokens and values for this term   
      $placeholders = pathauto_get_placeholders('taxonomy_menu', $term);

      if ($term->depth <= $old_depth) {
        $slashes_to_remove = $old_depth - $term->depth + 1;
        for ($i = 0; $i < $slashes_to_remove; $i++) {
          $old_path = substr($old_path, 0, strrpos($old_path, '/'));
        }
      }
      $path       = $old_path .'/'. $term->tid;
      $old_depth  = $term->depth;
      $old_path   = $path;
  
      if ($alias = pathauto_create_alias('taxonomy_menu', 'bulkupdate', $placeholders, $path, $term->tid, $category->vid)) {
        $count++;
      }
    }
  
    while ($category = db_fetch_object($result)) {
      $count += _taxonomy_pathauto_alias($category, 'bulkupdate');
    }
    
    drupal_set_message(format_plural($count,
      "Bulk generation of terms completed, one alias generated.",
      "Bulk generation of terms completed, @count aliases generated.")
    );
  }
}

/**
 * Implementation of hook_token_values()
 */
function taxonomy_menu_token_values($type, $object = NULL, $options = array()) {
  $values = array();
  switch ($type) {
    case 'taxonomy_menu':
      $category = $object;

      $vid = $category->vid;
      $vocabulary = taxonomy_vocabulary_load($vid);
      $values['tax-menu'] = variable_get('taxonomy_menu_display_page', 'category');
      $values['vid'] = $vid;
      $values['vocab'] = check_plain($vocabulary->name);
      $values['cat'] = check_plain($category->name);
      $values['tid'] = $category->tid;
      $values['vocab-raw'] = $vocabulary->name;
      $values['cat-raw'] = $category->name;

      break;
  }
  return $values;
}

/**
 * Implementation of hook_token_list()
 */
function taxonomy_menu_token_list($type = 'all') {
  if ($type == 'taxonomy_menu' || $type == 'all') {
    $tokens['taxonomy_menu']['tax-menu'] = t("The first item on the menu as defined <a href='@tax_menu_settings'>here</a>.", array('@tax_menu_settings' => url('admin/settings/taxonomy_menu')));
    $tokens['taxonomy_menu']['vid'] = t("The id number of the category's parent vocabulary.");
    $tokens['taxonomy_menu']['vocab'] = t("The vocabulary that the page's first category belongs to.");
    $tokens['taxonomy_menu']['cat'] = t('The name of the category.');
    $tokens['taxonomy_menu']['tid'] = t('The id number of the category.');
    $tokens['taxonomy_menu']['vocab-raw'] = t("The unfiltered vocabulary that the page's first category belongs to. WARNING - raw user input.");
    $tokens['taxonomy_menu']['cat-raw'] = t('The unfiltered name of the category. WARNING - raw user input.');

    return $tokens;
  }
}
