@lightning @layout @api @rc7
Feature: Panelizer Wizard

  Scenario: Landing page default layout has the proper Content context
    Given I am logged in as a user with the layout_manager role
    # Initialize the tempstor
    When I visit "/admin/structure/panelizer/edit/node__landing_page__default__default"
    # Then view the list of available contexts
    And I visit "/admin/structure/panels/panelizer.wizard/node__landing_page__default__default/select_block"
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
    When I visit "/admin/structure/types/manage/landing_page/display"
    And I check the box "Search result highlighting input"
    And I press "Save"
    And I visit "/admin/structure/types/manage/landing_page/display/search_result"
    And I check "Allow panelizer default choice"
    And I press "Save"
    And I visit "/node/add/landing_page"
    Then I should see "Full content"
    And I should see "Search result highlighting input"
    And I visit "/admin/structure/types/manage/landing_page/display"
    And I uncheck "Search result highlighting input"
    And I press "Save"
    And I visit "/node/add/landing_page"
    And I should not see "Search result highlighting input"

  @javascript
  Scenario: Switch between defined layouts.
    Given I am logged in as a user with the "landing_page_creator,layout_manager" roles
    And I visit "/admin/structure/panelizer/edit/node__landing_page__full__two_column/content"
    And I place the "Authored by" into the "Left side" panelizer region
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
    And I visit "/admin/structure/panelizer/edit/node__landing_page__full__two_column/content"
    #And I remove the "Authored by" block from the "Left side" panelizer region

