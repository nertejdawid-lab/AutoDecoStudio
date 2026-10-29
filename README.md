# Auto Deco Studio — Geode mod project (Windows / GD 2.2081)

**Status:** Source code. The pure C++ pattern generator has been tested on Linux; the Geode DLL and in-game editor workflow have NOT been compiled or tested here. Do not claim this ZIP is a ready .geode.

## How to use once built

1. Install resulting `community.auto-deco-studio.geode` into `Geometry Dash/geode/mods/`.
2. Open an **editable copy** of your level in the Geometry Dash editor.
3. Select supported solid blocks (not a trigger, orb, spike or portal).
4. Click **Theme** (at top of editor) to cycle Hell / Purple Hell / Heaven / Ice / Dark Monochrome. The current theme is shown below it.
5. Click **Generate**. The mod creates actual GD decorative objects using the game's native paste-in-place operation.
6. Save the level normally. You can undo the insertion using the editor's Undo control if this operation is registered by GD as a native paste.

## Colors

The prototype uses native in-level color channels `1` (main color) and `2` (accent / glow). For the four requested looks, set these channels in the GD level color settings:

| Style | Channel 1 | Channel 2 |
|---|---|---|
| Hell | #8B101B | #FF5036 |
| Purple Hell | #48144E | #D354F3 |
| Heaven / Ice | #5DB4DE | #E4F5FF |
| Dark Monochrome | #282B35 | #858A97 |

Theme selection changes **shape/layout** of decorative objects; because colors are shared level channels, it does not automatically overwrite your level palette. If you use channels 1 or 2 elsewhere, choose a test level or expect the existing level colors to be reused. This is a prototype, not a full AI visual reconstruction of the screenshots.

## Notes

- The generation is deliberately restricted to a whitelist of normal block IDs and has a 750-deco cap.
- Only non-solid / decorative IDs are generated; collision geometry should remain unchanged.
- Existing level elements are not deleted or recolored.
- Appearance depends on the blocks selected, the level palette, and your texture pack. Some decoration types may look different from the inspiration images.
- This code is not verified against the installed Geode SDK. If GitHub Actions fails, copy the error log; a source fix may be needed.

## GitHub Actions build

Create a **separate** repository (`AutoDecoStudio`) to avoid replacing your Input Buffer project. Upload all files and the hidden `.github` directory. Under Actions, run `Build Auto Deco Studio (Windows)`. On success, download artifact `AutoDecoStudio-Windows`, unzip, and install `.geode` into the game's `geode/mods` folder.

## Offline generator tests

```bash
cmake -S . -B build-tests -DAUTODECO_TEST_ONLY=ON
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```
