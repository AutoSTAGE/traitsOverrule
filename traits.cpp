//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2022 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
//
// traits.cpp
//
// This sample demonstrates using many of the AcGiSubEntityTraits
// class functions for controlling the properties of the graphics
// primitives drawn during entity elaboration.

#define AC_FULL_DEBUG
#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

//#include "arxHeaders.h"
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "aced.h"
#include "dbents.h"
#include "dbsymtb.h"
#include "dbapserv.h"
#include "acgi.h"
#include "tchar.h"
#include "AuStEntityOverrule.h"
#include "AuStOLayer.h"



// function prototypes
//
// THE FOLLOWING CODE APPEARS IN THE SDK DOCUMENT.

static Acad::ErrorStatus
getLinetypeIdFromString(const TCHAR* str, AcDbObjectId& id);
static Acad::ErrorStatus
getLayerIdFromString(const TCHAR* str, AcDbObjectId& id);

// END CODE APPEARING IN SDK DOCUMENT.

// Helpful constants for setting attributes:
// COLOR
//
static const Adesk::UInt16 kColorByBlock   = 0;
static const Adesk::UInt16 kRed            = 1;
static const Adesk::UInt16 kYellow         = 2;
static const Adesk::UInt16 kGreen          = 3;
static const Adesk::UInt16 kCyan           = 4;
static const Adesk::UInt16 kBlue           = 5;
static const Adesk::UInt16 kMagenta        = 6;
static const Adesk::UInt16 kWhite          = 7;
static const Adesk::UInt16 kColorByLayer   = 256;


class AsdkTraitsSamp: public AcDbEntity
{
public:

    ACRX_DECLARE_MEMBERS(AsdkTraitsSamp);
    AsdkTraitsSamp();
    AsdkTraitsSamp(AcDbObjectId layerOid, int iIndex);

    AcDbObjectId _layerOid;
    int _iIndex;

protected:
    Adesk::UInt32 subSetAttributes(AcGiDrawableTraits* pTraits) override;

    Adesk::Boolean subWorldDraw(AcGiWorldDraw *) override;
    Acad::ErrorStatus subTransformBy(const AcGeMatrix3d &) override;

    void asBtrEntityOverrule(
        AcGiCommonDraw *giDraw,
        AcDb::LineWeight *lwObject,
        AcCmTransparency &transp,
        Adesk::UInt16 &iEntityColor
    );
    void AsdkTraitsSamp::asBtrEntityOverruleOff();

    AcDbCircle m_circle1;
    AcDbCircle m_circle2;
};


ACRX_DXF_DEFINE_MEMBERS(AsdkTraitsSamp,AcDbEntity,
AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 0,\
    ASDKTRAITSSAMP,AsdkTraits Sample);

AsdkTraitsSamp::AsdkTraitsSamp() : AcDbEntity(),
    _layerOid(AcDbObjectId::kNull),
    _iIndex(0)
{
}
AsdkTraitsSamp::AsdkTraitsSamp(AcDbObjectId layerOid, int iIndex) : AcDbEntity(), 
    _layerOid(layerOid),
    _iIndex(iIndex)
{
}


Acad::ErrorStatus AsdkTraitsSamp::subTransformBy(
    const AcGeMatrix3d &xfm)
{
    return Acad::eOk;
}
 
// THE FOLLOWING CODE APPEARS IN THE SDK DOCUMENT.

/* use this to overrule entities */
void AsdkTraitsSamp::asBtrEntityOverrule(
    AcGiCommonDraw *giDraw,
    AcDb::LineWeight *lwObject,
    AcCmTransparency &transp,
    Adesk::UInt16 &iEntityColor
)
{
    /* set objectId to null to get layer from BTR entity */
    AcDbObjectId layerOid = AcDbObjectId::kNull;

    /* but use object layer for plot */
    /* declare explicitly isPlotGeneration() to make sure that we get the object layer id only for plot */
    if ((giDraw != nullptr) &&
        (giDraw->context()->isPlotGeneration() == Adesk::kTrue))
        layerOid = this->layerId();

    /* otherwise, we use the BTR entity layer >> layerOid is null */

    /* Acticates the overrule */
    AuStEntityOverrule::Activate();

    /* Set traits for overrule */
    AuStEntityOverrule::_pEntityOverrule->updateTraits(
        lwObject != nullptr ? *lwObject : AcDb::LineWeight(),
        transp,
        layerOid,
        iEntityColor
    );

} // END
void AsdkTraitsSamp::asBtrEntityOverruleOff()
{
    /* make sure the entity overrule is disabled after each call */
    /* the overrule object is still instantiated */
    AuStEntityOverrule::_pEntityOverrule->Disable();
}

Adesk::UInt32 AsdkTraitsSamp::subSetAttributes(AcGiDrawableTraits* pTraits)
{
    return AcDbEntity::subSetAttributes(pTraits) | kDrawableIsCompoundObject;
}

Adesk::Boolean
AsdkTraitsSamp::subWorldDraw(AcGiWorldDraw *pWd)
{
    AcGePoint3d InsPoint = AcGePoint3d::kOrigin;
    if (_iIndex == 1)
        InsPoint.x += 1000.0;

    /* use hidden layer with second entity */
    AcDbObjectId layerOid = this->layerId();
    if (_iIndex == 1)
        layerOid = _layerOid;

    //AcDbCircle *pCircle = new AcDbCircle();
    //pCircle->setDatabaseDefaults();
    //pCircle->setCenter(InsPoint);
    //pCircle->setRadius(1000.0);
    //pCircle->setColorIndex(1);
    //pCircle->setLayer(layerOid);

    m_circle1.setDatabaseDefaults();
    m_circle1.setCenter(InsPoint);
    m_circle1.setRadius(1000.0);
    m_circle1.setColorIndex(1);
    m_circle1.setLayer(layerOid, false, true);

    AcDb::LineWeight lwObject = AcDb::kLnWt211;
    AcCmTransparency transp; transp.setAlphaPercent(50);
    Adesk::UInt16 iEntityColor = 3;

    if (_iIndex == 1)
        /* call THIS to overrule properties after draw() */
        asBtrEntityOverrule(
            pWd,
            &lwObject,
            transp,
            iEntityColor
        );

    //pWd->rawGeometry()->draw(pCircle);
    pWd->rawGeometry()->draw(&m_circle1);

    //delete pCircle;
    //pCircle = nullptr;

    //AcDbCircle *pCircle2 = new AcDbCircle();
    //pCircle2->setDatabaseDefaults();
    //pCircle2->setCenter(InsPoint);
    //pCircle2->setRadius(500.0);
    //pCircle2->setColorIndex(1);
    //pCircle2->setLayer(layerOid);

    //pWd->rawGeometry()->draw(pCircle2);

    m_circle2.setDatabaseDefaults();
    m_circle2.setCenter(InsPoint);
    m_circle2.setRadius(500.0);
    m_circle2.setColorIndex(1);
    m_circle2.setLayer(layerOid, false, true);

    pWd->rawGeometry()->draw(&m_circle2);

    if (_iIndex == 1)
        /* make sure the entity overrule is deactivated */
        asBtrEntityOverruleOff();

    //delete pCircle2;
    //pCircle2 = nullptr;

    return Adesk::kTrue;
}


// A useful function that gets the linetype ID from the
// linetype's name -- must be in upper case.
// 
static Acad::ErrorStatus
getLinetypeIdFromString(const TCHAR* str, AcDbObjectId& id)
{
    Acad::ErrorStatus err;
 
    // Get the table of currently loaded line types.
    //
    AcDbLinetypeTable *pLinetypeTable;
    err = acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pLinetypeTable, AcDb::kForRead);
    if (err != Acad::eOk)
        return err;
 
    // Get the id of the linetype with the name that
    // 'str' contains.
    //
    err = pLinetypeTable->getAt(str, id, Adesk::kTrue);
 
    pLinetypeTable->close();
 
    return err;
}
 
 
// A useful function that gets the layer ID from the
// layer's name -- must be in upper case.
// 
static Acad::ErrorStatus
getLayerIdFromString(const TCHAR* str, AcDbObjectId& id)
{
    Acad::ErrorStatus err;
 
    // Get the table of currently loaded layers.
    //
    AcDbLayerTable *pLayerTable;
    err = acdbHostApplicationServices()->workingDatabase()
        ->getSymbolTable(pLayerTable, AcDb::kForRead);
    if (err != Acad::eOk)
        return err;
 
    // Get the ID of the layer with the name that 'str'
    // contains.
    //
    err = pLayerTable->getAt(str, id, Adesk::kTrue);
 
    pLayerTable->close();
 
    return err;
}

// END CODE APPEARING IN SDK DOCUMENT.

void
addAsdkTraitsSampObject()
{
    AcDbObjectId oIdHiddenLayer = AcDbObjectId::kNull;

    for (int i = 0; i <= 1; i++)
    {
        if (i == 1)
        {
            AuStLayerProperty layProp;
            layProp.setIsHidden(true);
            AcString layerName = _T("HIDDEN_LAYER");
            AuStOLayer::Layer(&layerName, oIdHiddenLayer, &layProp, AcDb::kForRead);
            if (oIdHiddenLayer == AcDbObjectId::kNull)
                return;
        }

        AsdkTraitsSamp *pNewObj = new AsdkTraitsSamp(oIdHiddenLayer, i);
        pNewObj->setDatabaseDefaults();

        AcDbObjectId oIdEntLayer = AuStOLayer::Layer(_T("entity_layer"), AcDb::kForRead);
        pNewObj->setLayer(oIdEntLayer);

        AcDbBlockTable *pBlockTable;
        acdbHostApplicationServices()->workingDatabase()
            ->getSymbolTable(pBlockTable, AcDb::kForRead);

        AcDbBlockTableRecord *pBlock;
        pBlockTable->getAt(ACDB_MODEL_SPACE, pBlock,
            AcDb::kForWrite);

        AcDbObjectId objId;
        pBlock->appendAcDbEntity(objId, pNewObj);

        pBlockTable->close();
        pBlock->close();
        pNewObj->close();
    }
}

static void initAsdkTraitsSamp()
{
    AsdkTraitsSamp::rxInit();
    acrxBuildClassHierarchy();

    acedRegCmds->addCommand(_T("ASDK_TRAITS_SAMP"),
                            _T("ASDKTRAITSSAMP"),
                            _T("TRAITSSAMP"),
                            ACRX_CMD_TRANSPARENT,
                            addAsdkTraitsSampObject);
}

extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId) 
{
    switch(msg) {
	case AcRx::kInitAppMsg:
        acrxDynamicLinker->unlockApplication(appId);
		acrxDynamicLinker->registerAppMDIAware(appId);
	    initAsdkTraitsSamp();
	    break;
	case AcRx::kUnloadAppMsg:
	    acedRegCmds->removeGroup(_T("ASDK_TRAITS_SAMP"));
            deleteAcRxClass(AsdkTraitsSamp::desc());
    }
    return AcRx::kRetOK;
}
