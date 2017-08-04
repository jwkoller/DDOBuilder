// ActiveEffect.cpp
//
#include "stdafx.h"
#include "ActiveEffect.h"

#include "Character.h"
#include "GlobalSupportFunctions.h"

//============================================================================
// ActiveEffect
ActiveEffect::ActiveEffect() :
    m_bonusType(Bonus_Unknown),
    m_type(ET_unknown),
    m_effectName("Not setup"),
    m_numStacks(0),
    m_amount(0),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(""),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        size_t stacks,
        double amount,
        const std::string & tree) :
    m_bonusType(bonusType),
    m_type(ET_amount),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(amount),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        double amount,
        const std::string & tree) :
    m_bonusType(bonusType),
    m_type(ET_amountPerAp),
    m_effectName(name),
    m_numStacks(0),
    m_amount(amount),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
    // stacks is set immediately after this is constructed
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        size_t stacks,
        const Dice & dice,
        const std::string & tree) :
    m_bonusType(bonusType),
    m_type(ET_dice),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),            // not used
    m_dice(dice),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        size_t stacks,
        const std::vector<double> & amounts) :
    m_bonusType(bonusType),
    m_type(ET_amountVector),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),                // not used
    m_amounts(amounts),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        double amountPerLevel,
        size_t stacks,
        ClassType classType) :
    m_bonusType(bonusType),
    m_type(ET_amountPerLevel),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),                // not used
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(amountPerLevel),
    m_class(classType)
{
}

BonusType ActiveEffect::Bonus() const
{
    return m_bonusType;
}

bool ActiveEffect::IsAmountPerAP() const
{
    return (m_type == ET_amountPerAp);
}

CString ActiveEffect::Name() const
{
    return m_effectName.c_str();
}

CString ActiveEffect::Stacks() const
{
    CString text;
    text.Format("%d", m_numStacks);
    return text;
}

CString ActiveEffect::AmountAsText() const
{
    CString text;
    switch (m_type)
    {
    case ET_dice:
        text.Format("%dD%d", m_dice.Number(), m_dice.Sides());
        // optional effects such as its Fire damage
        if (m_bHasEnergy)
        {
            text += " ";
            text += EnumEntryText(m_energy, energyTypeMap);
        }
        break;
    case ET_amount:
        text.Format("%.2f", m_amount * m_numStacks);
        break;
    case ET_amountVector:
        {
            size_t index = m_numStacks-1;
            if (index >= m_amounts.size())
            {
                index = m_amounts.size()-1;
                ::OutputDebugString("ActiveEffect ");
                ::OutputDebugString((LPCTSTR)Name());
                ::OutputDebugString(" has more stacks than amount vector\n");
            }
            text.Format("%.2f", m_amounts[index]);
        }
        break;
    case ET_amountPerLevel:
        text.Format("%.2f", m_amountPerLevel * m_numStacks);
        break;
    case ET_amountPerAp:
        text.Format("%.2f", m_amount * m_numStacks);
        break;
    default:
        text = "???";
        break;
    }
    return text;
}

double ActiveEffect::Amount() const
{
    return m_amount;
}

void ActiveEffect::SetAmount(double amount)
{
    m_amount = amount;
}

void ActiveEffect::SetEnergy(EnergyType type)
{
    m_bHasEnergy = (type != Energy_Unknown);
    m_energy = type;
}

void ActiveEffect::SetBreakdownDependency(BreakdownType bt)
{
    m_bt = bt;
}

bool ActiveEffect::HasBreakdownDependency(BreakdownType bt) const
{
    return (bt == m_bt);
}

bool ActiveEffect::HasClass(ClassType type) const
{
    if (m_type == ET_amountPerLevel)
    {
        return (type == m_class);
    }
    return false;
}

void ActiveEffect::AddStack()
{
    ++m_numStacks;
}

bool ActiveEffect::RevokeStack()
{
    --m_numStacks;
    return (m_numStacks == 0);  // true if no stacks left
}

void ActiveEffect::SetStacks(size_t count)
{
    m_numStacks = count;
}

double ActiveEffect::TotalAmount() const
{
    double value = 0.0;
    switch (m_type)
    {
    case ET_dice:
        break;
    case ET_amount:
        value = m_amount * m_numStacks;
        break;
    case ET_amountVector:
        {
            size_t index = m_numStacks-1;
            if (index >= m_amounts.size())
            {
                index = m_amounts.size()-1;
                ::OutputDebugString("ActiveEffect ");
                ::OutputDebugString((LPCTSTR)Name());
                ::OutputDebugString(" has more stacks than amount vector\n");
            }
            value = m_amounts[index];
        }
        break;
    case ET_amountPerLevel:
        value = m_amountPerLevel * m_numStacks;
        break;
    case ET_amountPerAp:
        value = m_amount * m_numStacks;
        break;
    default:
        value = 0.0;
        break;
    }
    return value;
}

void ActiveEffect::AddFeat(const std::string & featName)
{
    m_feats.push_back(featName);
}

void ActiveEffect::AddStance(const std::string & stance)
{
    m_stances.push_back(stance);
}

const std::vector<std::string> & ActiveEffect::Stances() const
{
    return m_stances;
}

const std::string & ActiveEffect::Tree() const
{
    return m_tree;
}

bool ActiveEffect::IsActive(const Character * pCharacter) const
{
    // return true if the required stance(s) are active
    bool active = true;
    for (size_t i = 0; i < m_stances.size(); ++i)
    {
        if (!pCharacter->IsStanceActive(m_stances[i]))
        {
            active = false;
        }
    }
    // check any dependent feats are trained
    for (size_t i = 0; i < m_feats.size(); ++i)
    {
        if (!pCharacter->IsFeatTrained(m_feats[i]))
        {
            active = false;
        }
    }
    return active;
}

bool ActiveEffect::operator<=(const ActiveEffect & other) const
{
    bool lessThanOrEqual = false;
    if (m_bonusType != Bonus_stacking)   // stacking items always stack
    {
        // must be the same type of bonus to allow a lessThan
        if (m_type == other.m_type
                //&& m_effectName == other.m_effectName
                && m_bonusType == other.m_bonusType)
        {
            // comes down to the amount field
            if (m_type == other.m_type)
            {
                lessThanOrEqual = (TotalAmount() <= other.TotalAmount());
            }
        }
    }
    return lessThanOrEqual;
}

bool ActiveEffect::operator==(const ActiveEffect & other) const
{
    bool equal = false;
    // must be the same type of bonus to allow a lessThan
    if (m_type == other.m_type
            && m_effectName == other.m_effectName
            && m_bonusType == other.m_bonusType)
    {
        switch (m_type)
        {
        case ET_dice:
            equal = (m_dice == other.m_dice);
            break;
        case ET_amount:
        case ET_amountPerAp:
            equal = (m_amount == other.m_amount);
            break;
        case ET_amountVector:
            equal = (m_amounts == other.m_amounts);
            break;
        case ET_amountPerLevel:
            equal = (m_amountPerLevel == other.m_amountPerLevel);
            break;
        default:
            equal = false;
            break;
        }
    }
    return equal;
}