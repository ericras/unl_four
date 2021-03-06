<?php

/**
 * Replacement for theme_webform_element().
 * 
 * Converts composite element labels into fieldsets, the label becomes a legend.
 * see https://drupal.org/node/1195418
 */
function unl_four_webform_element($variables)
{
  // Ensure defaults.
  $variables['element'] += array(
    '#title_display' => 'before',
  );

  $element = $variables['element'];

  // All elements using this for display only are given the "display" type.
  if (isset($element['#format']) && $element['#format'] == 'html') {
    $type = 'display';
  } else {
    $type = (isset($element['#type']) && !in_array($element['#type'], array('markup', 'textfield'))) ? $element['#type'] : $element['#webform_component']['type'];
  }
  $parents = str_replace('_', '-', implode('--', array_slice($element['#parents'], 1)));

  $wrapper_classes = array(
    'form-item',
    'webform-component',
    'webform-component-' . $type,
  );
  if (isset($element['#title_display']) && $element['#title_display'] == 'inline') {
    $wrapper_classes[] = 'webform-container-inline';
  }
  $output = '<div class="' . implode(' ', $wrapper_classes) . '" id="webform-component-' . $parents . '">' . "\n";

  // Composite elements consist of more than one HTML form control. These must be grouped by a fieldset.
  // Developers can mark any fieldtype as composite with #composite.
  // radios, checkboxes, and date are marked as composite. In D8, they are so marked in their respective form_process_*().
  if (isset($element['#type']) && in_array($element['#type'], array('radios', 'checkboxes', 'date', 'webform_grid', 'webform_time', 'managed_file'), true)) {
    $element['#composite'] = true;
  }

  $composite = false;
  if (isset($element['#composite']) && $element['#composite'] === true) {
    $composite = true;
  }

  if ($composite) {
    $output .= '<fieldset' . drupal_attributes(array('class' => 'fieldset-invisible')) . '>';
  }

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }
  if (!isset($element['#children'])) {
    $element['#children'] = '';
  }
  $prefix   = isset($element['#field_prefix']) ? '<span class="field-prefix">' . _webform_filter_xss($element['#field_prefix']) . '</span> ' : '';
  $suffix   = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . _webform_filter_xss($element['#field_suffix']) . '</span>' : '';
  $children = ' ' . $prefix . $element['#children'] . $suffix;

  // Generate the title, either a legend or a label, only if the title will be used
  if (!in_array($element['#title_display'], array('none', 'attribute'))) {
    if ($composite) {
      $attributes = array();
      if ($element['#title_display'] == 'invisible') {
        $attributes['class'] = 'element-invisible';
      }
      $title = '<legend' . drupal_attributes($attributes) . '>' . $element['#title'];
      if (!empty($element['#required'])) {
        $title .= theme('form_required_marker', array('element' => $element));
      }
      $title .= '</legend>';
    } else {
      $title = ' ' . theme('form_element_label', $variables);
    }
  }

  switch ($element['#title_display']) {
    case 'inline':
    case 'before':
    case 'invisible':
      $output .= $title . $children;
      break;

    case 'after':
      $output .= $children . $title;
      break;

    case 'none':
    case 'attribute':
      // Output no label and no required marker, only the children.
      $output .= $children;
      break;
  }
  $output .= "\n";

  if (!empty($element['#description'])) {
    $output .= '<div class="description">' . $element['#description'] . "</div>\n";
  }

  if ($composite) {
    $output .= '</fieldset>';
  }

  $output .= "</div>\n";

  return $output;
}

/**
 * Replacement for theme_form_element().
 * 
 * Moves invisible checkboxes and radio buttons to the right of the form control to pass WCAG2.0
 */
function unl_four_form_element($variables)
{
  $element = & $variables['element'];
  // This is also used in the installer, pre-database setup.
  $t = get_t();

  // This function is invoked as theme wrapper, but the rendered form element
  // may not necessarily have been processed by form_builder().
  $element += array(
    '#title_display' => 'before',
  );

  // Add element #id for #type 'item'.
  if (isset($element['#markup']) && !empty($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }
  // Add element's #type and #name as class to aid with JS/CSS selectors.
  $attributes['class'] = array('form-item');
  if (!empty($element['#type'])) {
    $attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $attributes['class'][] = 'form-item-' . strtr($element['#name'], array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $attributes['class'][] = 'form-disabled';
  }
  $output = '<div' . drupal_attributes($attributes) . '>' . "\n";

  // Composite elements consist of more than one HTML form control. These must be grouped by a fieldset.
  // Developers can mark any fieldtype as composite with #composite.
  // radios, checkboxes, and date are marked as composite. In D8, they are so marked in their respective form_process_*().
  if (isset($element['#type']) && in_array($element['#type'], array('radios', 'checkboxes', 'date'), true)) {
    $element['#composite'] = true;
  }
  $composite = false;
  if (isset($element['#composite']) && $element['#composite'] === true) {
    $composite = true;
  }

  if ($composite) {
    $output .= '<fieldset' . drupal_attributes(array('class' => 'fieldset-invisible')) . '>';
  }

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }
  if (!isset($element['#children'])) {
    $element['#children'] = '';
  }
  $prefix   = isset($element['#field_prefix']) ? '<span class="field-prefix">' . $element['#field_prefix'] . '</span> ' : '';
  $suffix   = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . $element['#field_suffix'] . '</span>' : '';
  $children = ' ' . $prefix . $element['#children'] . $suffix;

  // Generate the title, either a legend or a label, only if the title will be used
  if (!in_array($element['#title_display'], array('none', 'attribute'))) {
    if ($composite) {
      $attributes = array();
      if ($element['#title_display'] == 'invisible') {
        $attributes['class'] = 'element-invisible';
      }
      $title = '<legend' . drupal_attributes($attributes) . '>' . $element['#title'];
      if (!empty($element['#required'])) {
        $title .= theme('form_required_marker', array('element' => $element));
      }
      $title .= '</legend>';
    } else {
      $title = ' ' . theme('form_element_label', $variables);
    }
  }

  switch ($element['#title_display']) {
    case 'before':
    case 'invisible':
      if ((isset($element['#type']) && in_array($element['#type'], array('radio', 'checkbox')))) {
        //Labels need to be placed after raidos and checkboxes - WCAG2.0 H44
        $output .= $children . $title;
      } else {
        $output .= $title . $children;
      }
      //$output .= $title . $children;
      break;
    case 'after':
      $output .= $children . $title;
      break;

    case 'none':
    case 'attribute':
      // Output no label and no required marker, only the children.
      $output .= $children;
      break;
  }
  $output .= "\n";

  if (!empty($element['#description'])) {
    $output .= '<div class="description">' . $element['#description'] . "</div>\n";
  }

  if ($composite) {
    $output .= '</fieldset>';
  }

  $output .= "</div>\n";

  return $output;
}

/**
 * Theme a webform time element.
 * 
 * Adds labels so that it passes WCAG2.0
 */
function unl_four_webform_time($variables) {
  $element = $variables['element'];

  $element['hour']['#attributes']['class'] = array('hour');
  $element['minute']['#attributes']['class'] = array('minute');

  // Add error classes to all items within the element.
  if (form_get_error($element)) {
    $element['hour']['#attributes']['class'][] = 'error';
    $element['minute']['#attributes']['class'][] = 'error';
  }

  $element['hour']['#title']           = 'New Hour';
  $element['hour']['#title_display']   = 'invisible';
  $element['minute']['#title']         = 'New Minute';
  $element['minute']['#title_display'] = 'invisible';

  $output = '<div class="webform-container-inline">' . drupal_render($element['hour']) . drupal_render($element['minute']) . drupal_render($element['ampm']) . '</div>';

  return $output;
}

/**
 * implements theme_file
 * 
 * Renders the file element via core, then passes it to the theme wrapper, which adds labels.
 * 
 * @param $vars
 *
 * @return string
 */
function unl_four_file($vars) {
  //Use the default theme_file
  $vars['element']['#children'] = theme_file($vars);
  //theme it to add labels
  return theme('form_element', $vars);
}