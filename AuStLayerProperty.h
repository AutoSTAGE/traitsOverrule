#ifndef _AUSTLAYERPROPERTY_H
#define _AUSTLAYERPROPERTY_H

//-----------------------------------------------------------------------------
//----- Declaration of AuStLayerProperty
//-----------------------------------------------------------------------------

#include "dbmain.h"


/////////////////////////////////////////////////
// Enum
//
// ** LayerState **
enum class LayerState
{
    eUnused = -1,
    eFalse = 0,
    eTrue = 1
};


class AuStLayerProperty
{

private:
    /////////////////////////////////////////////////
    // Class Version
    //
    // ** AuStLayerProperty **
    LayerState  _IsHidden;
    LayerState  _IsFrozen;
    LayerState  _IsOff;
    LayerState  _IsLocked;
    LayerState  _PrintEnabled;
    LayerState  _SkipHidden;

    LayerState  _ColorIndex;
    int         _iColorIndex;
    AcCmColor   _Color;


public:
    /////////////////////////////////////////////////
    // Constructor, Destructor, Init
    //
    // ** AuStLayerProperty **
    AuStLayerProperty();
    ~AuStLayerProperty();


public:
    /////////////////////////////////////////////////
    // Get Class Members
    //
    // ** AuStLayerProperty **
    
    /* returns FALSE if state is eUnused */
    bool IsHidden();
    bool IsFrozen();
    bool IsOff();
    bool IsLocked();
    bool PrintEnabled();

    /* Preset is TRUE */
    bool SkipHidden();
    
    /* Preset is FALSE >> Returns preset color value */
    int ColorIndex();
    AcCmColor Color();


public:
    /////////////////////////////////////////////////
    // Set Class Members
    //
    // ** AuStLayerProperty **
    void setIsHidden(const bool v);
    void setIsFrozen(const bool v);
    void setIsOff(const bool v);
    void setIsLocked(const bool v);
    void setPrintEnabled(const bool v);
    void setSkipHidden(const bool v);
    void setColorIndex(const int v);
    void setColor(const AcCmColor v);
    void setColor(const bool v);


public:
    /////////////////////////////////////////////////
    // PUBLIC Class Methods
    //
    // ** AuStLayerProperty **
    void EditProperties(
        AcDbLayerTableRecord *pLTR,
        AcDb::OpenMode &openMode
    );


private:
    /////////////////////////////////////////////////
    // PRIVATE Class Methods
    //
    // ** AuStLayerProperty **
    LayerState Get(bool v);
    bool Get(LayerState v);

    LayerState Set(bool v);


}; // class

#endif //_AUSTLAYERPROPERTY_H