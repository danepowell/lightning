@lightning @layout @api @rc7
Feature: Panelizer Wizard

  Scenario: Landing page default layout has the proper Content context
    Given I am logged in as a user with the layout_manager role
    # Initialize the tempstor
    When I visit "/admin/structure/panelizer/edit/node__landing_page__default__default"
    # Then view the list of available contexts
    And I visit "/admin/structure/panels/panelizer.wizard/node__landing_page__default__default/select_block"
    # @phpnotice: Undefined offset: 1 in Drupal\panels\Controller\Panels->getCachedValues() (line 15 of profiles/lightning/modules/contrib/panels/src/CachedValuesGetterTrait.php)
    Then I should see "Authored by"

  @javascript
  Scenario: Saving a panelized entity should not affect blocks places via the IPE
    Given I am logged in as a user with the "access panels in-place editing,administer panelizer node landing_page content,edit any landing_page content,view any unpublished content,use draft_draft transition,view latest version,access user profiles" permissions
    And landing_page content:
      | title  | path    | moderation_state |
      | Foobar | /foobar | draft            |
    When I visit "/foobar"
    And I place the "views_block:who_s_online-who_s_online_block" block from the "Lists (Views)" category
    # Click IPE Save
    And I save the layout
    And I click "Edit draft"
    And I press "Save"
    Then I should see a "views_block:who_s_online-who_s_online_block" block

  Scenario: The default layout switcher is available on entity edit forms for each view mode that has the "Allow panelizer default choice" optioned enabled, and only those view modes.
    Given I am logged in as a user with the "landing_page_creator,layout_manager" roles
    When I customize the search_result view mode of the landing_page content type
    And I visit "/admin/structure/types/manage/landing_page/display/search_result"
    And I check "Allow panelizer default choice"
    And I press "Save"
    And I visit "/node/add/landing_page"
    Then I should see a "Full content" field
    And I should see a "Search result highlighting input" field
    And I uncustomize the search_result view mode of the landing_page content type
    And I visit "/node/add/landing_page"
    # TODO: There's no way to assert that the *field* doesn't exist!
    And I should not see "Search result highlighting input"

  @javascript
  Scenario: Switch between defined layouts.
    Given I am logged in as a user with the "landing_page_creator,layout_manager" roles
    And I visit "/admin/structure/panelizer/edit/node__landing_page__full__two_column/content"
    And I place the "Authored by" into the "Left side" panelizer region
    # @phpnotice: Undefined offset: 1 in Drupal\panels\Form\PanelsBlockConfigureFormBase->getCachedValues() (line 15 of profiles/lightning/modules/contrib/panels/src/CachedValuesGetterTrait.php).
    And I press "Update and save"
    And landing_page content:
      | title  | path    | moderation_state |
      | Foobar | /foobar | draft            |
    When I visit "/foobar"
    And I click "Edit draft"
    And I select "Two Column" from "Full content"
    And press "Save"
    Then I should see "Authored by"
    And I click "Edit draft"
    And I select "Single Column" from "Full content"
    And press "Save"
    And I should not see "Authored by"
    And I remove "Authored by" from the left region of the two_column layout of the landing_page content type's full view mode
