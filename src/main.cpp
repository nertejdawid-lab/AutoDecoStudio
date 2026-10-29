#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "DecoPlanner.hpp"
#include <vector>
#include <algorithm>

using namespace geode::prelude;
using namespace cocos2d;

class $modify(AutoDecoEditorUI, EditorUI) {
    struct Fields {
        autodeco::Theme theme = autodeco::Theme::Hell;
        CCLabelBMFont* themeLabel = nullptr;
    };

    static CCMenuItemSpriteExtra* makeButton(CCObject* target, SEL_MenuHandler callback,
                                               char const* label) {
        auto bg = ButtonSprite::create(label, 110, 0, .8f, true,
                                       "goldFont.fnt", "GJ_button_01.png", 0.f);
        bg->setScale(.70f);
        return CCMenuItemSpriteExtra::create(bg, target, callback);
    }

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        auto menu = CCMenu::create();
        menu->setID("autodeco-menu"_spr);
        auto win = CCDirector::sharedDirector()->getWinSize();
        menu->setPosition({win.width / 2.f - 100.f, win.height - 25.f});
        menu->addChild(makeButton(this, menu_selector(AutoDecoEditorUI::onTheme), "Theme"));
        menu->addChild(makeButton(this, menu_selector(AutoDecoEditorUI::onGenerate), "Generate"));
        menu->alignItemsHorizontallyWithPadding(4.f);
        this->addChild(menu, 200);
        auto label = CCLabelBMFont::create("Hell", "goldFont.fnt");
        label->setScale(.38f);
        label->setPosition({win.width - 100.f, win.height - 48.f});
        label->setID("autodeco-theme-label"_spr);
        this->addChild(label, 201);
        m_fields->themeLabel = label;
        return true;
    }

    void onTheme(CCObject*) {
        auto next = (static_cast<int>(m_fields->theme) + 1) % 4;
        m_fields->theme = static_cast<autodeco::Theme>(next);
        if (m_fields->themeLabel)
            m_fields->themeLabel->setString(autodeco::themeName(m_fields->theme));
    }

    void onGenerate(CCObject*) {
        if (!m_editorLayer || m_isPaused) return;
        std::vector<autodeco::Seed> selected;
        selected.reserve(100);
        if (m_selectedObject) {
            selected.push_back({m_selectedObject->m_objectID,
                                m_selectedObject->getPositionX(), m_selectedObject->getPositionY()});
        }
        if (m_selectedObjects) {
            for (auto* o : geode::cocos::CCArrayExt<GameObject, false>(m_selectedObjects)) {
                if (!o || o == m_selectedObject) continue;
                selected.push_back({o->m_objectID, o->getPositionX(), o->getPositionY()});
                if (selected.size() >= 180) break;
            }
        }
        auto opts = Mod::get();
        int strength = opts->getSettingValue<int64_t>("intensity");
        bool glow = opts->getSettingValue<bool>("glow");
        bool air = opts->getSettingValue<bool>("air");
        auto planned = autodeco::generate(selected, m_fields->theme, strength, glow, air);
        if (planned.empty()) {
            FLAlertLayer::create("Auto Deco", "Select normal blocks first (not triggers or portals).", "OK")->show();
            return;
        }
        // Vanilla paste-in-place path creates editor objects with an undo entry.
        auto serialized = autodeco::toLevelString(planned);
        this->pasteObjects(serialized.c_str(), false, false);
        this->updateButtons();
        this->updateObjectInfoLabel();
        FLAlertLayer::create("Auto Deco", "Added decoration. Set color channels 1 and 2 to your theme palette.", "OK")->show();
    }
};
