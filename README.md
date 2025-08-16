# UnityCppDumper
This project is still **WIP**. It's a runtime dumper that was initially designed for [BA-Cheeto](https://github.com/Taiga74164/BA-Cheeto) and is supposed to be used along side [UnityResolve](https://github.com/issuimo/UnityResolve.hpp)

## Example dump:
``` c++
// Assembly: BlueArchive.dll
// Namespace: MX.Logic.Skills.Passive
// Class Flags: 0x100001 (Public)
// Instance Size: 96 bytes
// Is Abstract: false
// Is Interface: false
class BattleEntitySkillStartExecution : public PassiveExecution {
    UNITY_CLASS_DECL("BlueArchive.dll", "BattleEntitySkillStartExecution")

public:
    // Fields
    // Offset: 0x50, Flags: 0x21, Type: System.Collections.Generic.List<MX.Logic.Actions.HeroAction> (ReadOnly)
    UNITY_FIELD(HeroAction*, targetActions_k_BackingField, 0x50)
    // Offset: 0x58, Flags: 0x21, Type: System.Collections.Generic.List<MX.Logic.Skills.SkillType> (ReadOnly)
    UNITY_FIELD(SkillType*, SkillTypeConstraints_k_BackingField, 0x58)

public:
    // Methods
    // Flags: 0x81, RVA: 0x1035580, ParamCount: 2, ReturnType: System.Void
    // Parameters: UnityResolve::UnityType::Object* sender, EventArgs* e
    UNITY_METHOD(void, Action_Started, UnityResolve::UnityType::Object*, EventArgs*)
    // Flags: 0xc6, RVA: 0x1035670, ParamCount: 1, ReturnType: System.Void (Virtual)
    // Parameters: Battle* battle
    UNITY_METHOD(void, Disable, Battle*)
    // Flags: 0x881, RVA: 0x1035a30, ParamCount: 0, ReturnType: System.Collections.Generic.List<MX.Logic.Skills.SkillType>
    UNITY_METHOD(SkillType*, get_SkillTypeConstraints)
    // Flags: 0x881, RVA: 0x1035a40, ParamCount: 0, ReturnType: System.Collections.Generic.List<MX.Logic.Actions.HeroAction>
    UNITY_METHOD(HeroAction*, get_targetActions)
    // Flags: 0x81, RVA: 0x1035a50, ParamCount: 2, ReturnType: System.Void
    // Parameters: UnityResolve::UnityType::Object* sender, SkillActionRegisteredEventArgs* e
    UNITY_METHOD(void, BattleEntity_SkillActionRegistered, UnityResolve::UnityType::Object*, SkillActionRegisteredEventArgs*)
    // Flags: 0x81, RVA: 0x1035a70, ParamCount: 1, ReturnType: System.Void
    // Parameters: HeroAction* action
    UNITY_METHOD(void, RegisterTrigger, HeroAction*)
    // Flags: 0xc6, RVA: 0x1035ed0, ParamCount: 1, ReturnType: System.Void (Virtual)
    // Parameters: Battle* battle
    UNITY_METHOD(void, Enable, Battle*)

};
```

##  TODO:
- Handle generics properly
- Fixes for mono