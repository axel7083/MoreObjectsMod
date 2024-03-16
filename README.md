# More Objects Mod
Toolkit and resources for SM64DS for some custom objects.

## Installation

Inside this repository is a `commands.sm64ds` file which can be used with SM64DSe-Ultimate to insert, compile assets and dynamics libraries.

## Using Dynamics libaries

You need to download this repository to your computer. And have installed the latest version of [SM64DSe-Ultimate](https://github.com/Gota7/SM64DSe-Ultimate).

Inside the editor go to `More -> Import Patch` and select the `refactorTest.sp2` file from this repository.
Repeat the process once it finishes with `CompileTest.sp2`.

> ⚠️ The file `Code/include/List/FileList.h` and `Code/include/List/SoundList.h` are **generated** by the Editor when using the CompileTest.sp2 script.

> 🚨 The `SM64DSe/objectdb.xml` is **not** updated therefore **cannot** be used.

## Custom Objects

- 🚧: WIP
- ✅: Implemented as Dynamic Library
- ❓: Unknown status
- 🚩: Compile but crash at runtime

| Name                                                     | Screenshot | Status |
|----------------------------------------------------------|------------|--------|
| Galaxy Shrinking Platform                                |            | ✅ |
| CosmicClones                                             | <img width="176" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/6e88b033-499f-43c1-b61d-9ac0fbe022c3"> | ✅ |
| Rideable Yoshis                                          | <img width="147" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/33ee90a0-f957-493d-9cc3-f30cf78529d4"> | ✅ |
| Berries                                                  | <img width="152" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/580861ed-3474-4c1d-8e29-2de31a7f1dd0">  | ✅ |
| Invisible Walls                                          |            | ✅ |
| Silver Coins (Code/source/O_ColoredCoin)                 |            | ✅ |
| Object Lighting Modifier                                 |            | ✅ |
| Toxic Levels                                             |            | ✅ |
| 6 Types of Colored Goombas                               |            | 🚧 |
| Noteblocks                                               | <img width="145" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/fb7dc267-8036-4c7f-9503-253d2e3efb5f"> | ✅ |
| Shy Guys                                                 |            | ✅ |
| Magikoopa & Kamella                                      |            | ✅ |
| Launch Stars & Star Chips                                |            | ✅ |
| Skybox Rotator                                           |            | ✅ |
| Gravity Modifier                                         |            | ❓ |
| Yoshi NPC's                                              |            | ✅ |
| Colored Warp Pipes                                       | <img width="160" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/d35cef1b-3dfb-435d-9236-1bb8a528b31d"> | ✅ |
| Character Blocks                                         | <img width="239" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/d76becd5-e655-4fc9-afeb-ccd3e6ff0c40"> | ✅ |
| Tree Shadows                                             |            | ✅ |
| Save Blocks                                              | <img width="181" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/c999e2ff-c3e6-462f-a763-da38b4e83bb0"> | ✅ |
| Door Blockers                                            | <img width="185" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/55474b47-b044-4256-9e03-ddd73ca88212"> | ✅ |
| Colored Coins                                            | <img width="185" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/b5530b1e-a175-4350-b197-bd54237520ba"> | ✅ |
| Colored Toads                                            |            | ❓ |
| Peach NPC                                                | <img width="178" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/b2a8134f-19e8-4778-aab0-7035d7d53188"> | ✅ |
| Blue Ice Block                                           |            | ❓ |
| Mega Block                                               | <img width="229" alt="image" src="https://github.com/axel7083/MoreObjectsMod/assets/42176370/97220612-68a5-4d2f-826c-e3775d95c1bd"> | ✅ |
| Cutscene Loader                                          |            | 🚧 |
| Custom Block                                             |            | 🚧 |
| Treasure Chest                                           |            | 🚩 |
| Thwomp & Magma Thwomp                                    |            | 🚧 |
| 2D Level (With Camera Limiter)                           |            | ❓ |
| FlipSwitch  | ![FlipSwitch.png](Screenshots/FlipSwitch.png) | ✅ |

### Not working objects

| name | description                                                                                                                   |
| --- |-------------------------------------------------------------------------------------------------------------------------------|
| Treasure Chest (Code/source/O_TreasureChest) | Not working THUMB9: Undefined instruction: 0x0000BBF8 PC=0xE40000D4</br>ARM9: Undefined instruction: 0x0000BBF8 PC=0xE40000D4 |


## Credits
Gota7 - Mod, custom objects, sound and resource management.

Josh - Custom objects.

Splatterboy - Custom objects. Porting to Dyanmics libraries

Starpants - Misc. ASM, General help.

Overblade - RE help.

0reo - Idea.
