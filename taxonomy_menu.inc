<?php
/**
 * $Id$
 * @file taxonomy_menu.inc
 * @author Jonathan Chaffer   <jchaffer@structureinteractive.com>
 * @author Bruno Massa        <http://drupal.org/user/67164>
 * It Generates menu links for all taxonomy terms
 */

/**
 * Admin area. Configure the module, setting which
 * vocabularies will be converted into menus items
 *
 * @return
 *   Array. The form fields.
 */
function _taxonomy_menu_admin() {
  $options = array(
    taxonomy_menu_NONE    => t('No'),
    taxonomy_menu_NORMAL  => t('Normal')
  );

  // If the Views module is enabled, add some special
  // new features
  if (module_exists('views')) {
    // Add the Views file with more functions
    require_once(drupal_get_path('module', 'views') .'/views_cache.inc');

    // Add a new options on Categories
    $options[taxonomy_menu_VIEW] = t('Views');

    // Get the list of User generated views
    $views = db_query("SELECT * FROM {view_view}");
    while ($view = db_fetch_array($views)) {
      $views_list[$view['name']] = $view['page_title'];
    }

    // Now get a list of default Views
    foreach (_views_get_default_views() as $view => $viewdata) {
      $views_list[$view] = $viewdata->name;
    }
  }

  // Create some options for each of the vocabularies
  foreach (taxonomy_get_vocabularies() as $vocab) {
    $form[$vocab->vid] = array(
      '#title'          => $vocab->name,
      '#tree'           => FALSE,
      '#type'           => 'fieldset'
    );
    $form[$vocab->vid]['taxonomy_menu_show_'. $vocab->vid] = array(
      '#default_value'  => variable_get('taxonomy_menu_show_'. $vocab->vid, taxonomy_menu_NONE),
      '#options'        => $options,
      '#title'          => t('Show this category in menu'),
      '#type'           => 'radios'
    );

    // In case of View options selected, select Views
    if (module_exists('views')) {
      $form[$vocab->vid]['taxonomy_menu_show_view_'. $vocab->vid] = array(
        '#default_value'  => variable_get('taxonomy_menu_show_view_'. $vocab->vid, ''),
        '#options'        => $views_list,
        '#title'          => t('Views available'),
        '#type'           => 'select'
      );
    }
  }

  // General options
  $form['taxonomy_menu_display_page'] = array(
    '#default_value'  => variable_get('taxonomy_menu_display_page', 'category'),
    '#description'    => t('How should be the first item on the menu? Example: categories/1/2/3, technology/1/2/3'),
    '#title'          => t('Module page'),
    '#type'           => 'textfield',
  );
  $form['taxonomy_menu_display_num'] = array(
    '#default_value'  => variable_get('taxonomy_menu_display_num', FALSE),
    '#description'    => t('If checked, number of node per term will be displayed in the menu.'),
    '#title'          => t('Display number of nodes per terms'),
    '#type'           => 'checkbox',
  );
  $form['taxonomy_menu_hide_empty'] = array(
    '#default_value'  => variable_get('taxonomy_menu_hide_empty', FALSE),
    '#description'    => t('If checked, only taxonomy terms with members will be shown in the menu.'),
    '#title'          => t('Hide Empty Terms'),
    '#type'           => 'checkbox',
  );
  $form['taxonomy_menu_display_descendants'] = array(
    '#default_value'  => variable_get('taxonomy_menu_display_descendants', TRUE),
    '#description'    => t('If checked, then when a term is selected all nodes belonging to subterms are also displayed.'),
    '#title'          => t('Display descendants'),
    '#type'           => 'checkbox',
  );

  $form['submit'] = array(
    '#type'           => 'submit'
  );

  return $form;
}