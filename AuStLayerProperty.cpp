#include "dbsymtb.h"
#include "dbmain.h"
#include "AuStLayerProperty.h"

//#include "arxHeaders.h"


///////////////////////////////////////////////////////////////////////////////
// Constructor
//
// ** AuStLayerProperty **
AuStLayerProperty::AuStLayerProperty()
{
    _IsHidden = LayerState::eUnused;
	_IsFrozen = LayerState::eUnused;
	_IsOff = LayerState::eUnused;
	_IsLocked = LayerState::eUnused;
	_PrintEnabled = LayerState::eTrue;

    _SkipHidden = LayerState::eTrue;

    _ColorIndex = LayerState::eFalse;
    _iColorIndex = 7;
    _Color.setColorIndex(_iColorIndex);
}
AuStLayerProperty::~AuStLayerProperty()
{
}


/////////////////////////////////////////////////
// Get Class Members
//
// ** AuStLayerProperty **
bool AuStLayerProperty::IsHidden()
{
    return Get(_IsHidden);
}
bool AuStLayerProperty::IsFrozen()
{
    return Get(_IsFrozen);
}
bool AuStLayerProperty::IsOff()
{
    return Get(_IsOff);
}
bool AuStLayerProperty::IsLocked()
{
    return Get(_IsLocked);
}
bool AuStLayerProperty::PrintEnabled()
{
	return Get(_PrintEnabled);
}
bool AuStLayerProperty::SkipHidden()
{
	return Get(_SkipHidden);
}
int AuStLayerProperty::ColorIndex()
{
    return _iColorIndex;
}
AcCmColor AuStLayerProperty::Color()
{
    return _Color;
}


/////////////////////////////////////////////////
// Set Class Members
//
// ** AuStLayerProperty **
void AuStLayerProperty::setIsHidden(const bool v)
{
    _IsHidden = Set(v);
}
void AuStLayerProperty::setIsFrozen(const bool v)
{
    _IsFrozen = Set(v);
}
void AuStLayerProperty::setIsOff(const bool v)
{
    _IsOff = Set(v);
}
void AuStLayerProperty::setIsLocked(const bool v)
{
    _IsLocked = Set(v);
}
void AuStLayerProperty::setPrintEnabled(const bool v)
{
    _PrintEnabled = Set(v);
}
void AuStLayerProperty::setSkipHidden(const bool v)
{
    _SkipHidden = Set(v);
}
void AuStLayerProperty::setColorIndex(const int v)
{
    _ColorIndex = LayerState::eTrue;
	_iColorIndex = v;
	_Color.setColorIndex(v);
}
void AuStLayerProperty::setColor(const AcCmColor v)
{ 
    _ColorIndex = LayerState::eTrue;
    _Color = v;
    _iColorIndex = _Color.colorIndex();
}
void AuStLayerProperty::setColor(const bool v)
{
    _ColorIndex = Set(v);
}


/////////////////////////////////////////////////
// PUBLIC Class Methods
//
// ** AuStLayerProperty **
void AuStLayerProperty::EditProperties(
    AcDbLayerTableRecord *pLTR,
    AcDb::OpenMode &openMode
)
{
    /* get Layer State */
    if ((openMode == AcDb::kForRead) ||
        (openMode == AcDb::kForNotify))
    {
        /* get state */
        if (_IsFrozen != LayerState::eUnused)       setIsFrozen(pLTR->isFrozen());
        if (_IsOff != LayerState::eUnused)          setIsOff(pLTR->isOff());
        if (_IsLocked != LayerState::eUnused)       setIsLocked(pLTR->isLocked());
        if (_PrintEnabled != LayerState::eUnused)   setPrintEnabled(pLTR->isPlottable());

        /* get color */
        if (_ColorIndex != LayerState::eUnused)     setColor(pLTR->color());
    }

    /* set layer state */
    else if ((openMode == AcDb::kForWrite) &&
            (pLTR->upgradeOpen() == Acad::eOk))
    {
        /* set state */
        if (_IsFrozen != LayerState::eUnused)         pLTR->setIsFrozen(IsFrozen());
        if (_IsOff != LayerState::eUnused)            pLTR->setIsOff(IsOff());
        if (_IsLocked != LayerState::eUnused)         pLTR->setIsLocked(IsLocked());
        if (_PrintEnabled != LayerState::eUnused)     pLTR->setIsPlottable(PrintEnabled());

        /* set color */
        if (_ColorIndex != LayerState::eUnused)       pLTR->setColor(Color());
    }

} // END


/////////////////////////////////////////////////
// PRIVATE Class Methods
//
// ** AuStLayerProperty **
LayerState AuStLayerProperty::Get(bool v)
{
    if (v)
        return LayerState::eTrue;
    return LayerState::eFalse;
}
bool AuStLayerProperty::Get(LayerState v)
{
    if (v == LayerState::eTrue)
        return true;
    return false;
}

LayerState AuStLayerProperty::Set(bool v)
{
    if (v)
        return LayerState::eTrue;
    return LayerState::eFalse;
}