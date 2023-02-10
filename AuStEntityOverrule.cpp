
//-----------------------------------------------------------------------------
//----- AuStEntityOverrule.cpp : Implementation of AuStEntityOverrule
//-----------------------------------------------------------------------------

//#include "arxHeaders.h"
#include "acgi.h"
#include "dbmain.h"
#include "dbents.h"
#include "drawable.h"
#include "dbmtext.h"
#include "tchar.h"
#include "AuStEntityOverrule.h"
#include "AuStOLayer.h"


///////////////////////////////////////////////////////////////////////////////                                                       
//
// ** AuStSubEntityTraits **
class AuStSubEntityTraits : public AcGiSubEntityTraits
{
public:
    AuStSubEntityTraits(AcGiSubEntityTraits* pTraits) : m_pTraits(pTraits) {}

    /* no - ops so that entity can't change traits */
    void                    setColor(const Adesk::UInt16 color) override {}
    void                    setTrueColor(const AcCmEntityColor& color) override {}
    void                    setTransparency(const AcCmTransparency& transparency) override {};
    void                    setLineWeight(const AcDb::LineWeight lw) override {};
    void                    setLineWeight(const AcDb::LineWeight lw, const char lweight_index) override {};

    /* forwarders */
    void                    setLayer(const AcDbObjectId layerId) override { m_pTraits->setLayer(layerId); }
    void                    setLineType(const AcDbObjectId linetypeId) override { m_pTraits->setLineType(linetypeId); }
    void                    setSelectionMarker(const Adesk::LongPtr markerId) override { m_pTraits->setSelectionMarker(markerId); }
    void                    setFillType(const AcGiFillType type) override { m_pTraits->setFillType(type); }
    void                    setLineTypeScale(double dScale = 1.0) override { m_pTraits->setLineTypeScale(dScale); }
    void                    setThickness(double dThickness) override { m_pTraits->setThickness(dThickness); }
    void                    setVisualStyle(const AcDbObjectId visualStyleId) override { m_pTraits->setVisualStyle(visualStyleId); }
    void                    setPlotStyleName(
        AcDb::PlotStyleNameType type,
        const AcDbObjectId& id = AcDbObjectId::kNull) override
    {
        m_pTraits->setPlotStyleName(type, id);
    }
    void                    setMaterial(const AcDbObjectId materialId) override { m_pTraits->setMaterial(materialId); }
    void                    setMapper(const AcGiMapper* mapper) override { m_pTraits->setMapper(mapper); }
    void                    setSectionable(bool bSectionable) override { m_pTraits->setSectionable(bSectionable); }
    Acad::ErrorStatus       setDrawFlags(Adesk::UInt32 flags) override { return m_pTraits->setDrawFlags(flags); }
    Acad::ErrorStatus       setShadowFlags(ShadowFlags flags) override { return m_pTraits->setShadowFlags(flags); }
    void                    setSelectionGeom(bool bSelectionflag) override { m_pTraits->setSelectionGeom(bSelectionflag); }
    void                    setFill(const AcGiFill* pFill) override { m_pTraits->setFill(pFill); }

    Adesk::UInt16           color(void) const override { return m_pTraits->color(); }
    AcCmEntityColor         trueColor(void) const override { return m_pTraits->trueColor(); }
    AcDbObjectId            layerId(void) const override { return m_pTraits->layerId(); }
    AcDbObjectId            lineTypeId(void) const override { return m_pTraits->lineTypeId(); }
    AcGiFillType            fillType(void) const override { return m_pTraits->fillType(); }
    AcDb::LineWeight        lineWeight(void) const override { return m_pTraits->lineWeight(); }
    double                  lineTypeScale(void) const override { return m_pTraits->lineTypeScale(); }
    double                  thickness(void) const override { return m_pTraits->thickness(); }
    AcDbObjectId            visualStyle(void) const override { return m_pTraits->visualStyle(); }
    AcDb::PlotStyleNameType getPlotStyleNameId(AcDbObjectId& idResult) const override { return m_pTraits->getPlotStyleNameId(idResult); }
    AcDbObjectId            materialId(void) const override { return m_pTraits->materialId(); }
    const AcGiMapper*       mapper(void) const override { return m_pTraits->mapper(); }
    bool                    sectionable(void) const override { return m_pTraits->sectionable(); }
    Adesk::UInt32           drawFlags(void) const override { return m_pTraits->drawFlags(); }
    ShadowFlags             shadowFlags(void) const override { return m_pTraits->shadowFlags(); }
    bool                    selectionGeom(void) const override { return m_pTraits->selectionGeom(); }
    AcCmTransparency        transparency(void) const override { return m_pTraits->transparency(); }
    const AcGiFill*         fill(void) const override { return m_pTraits->fill(); }

    void                    pushMarkerOverride(
        bool flag,
        const Adesk::LongPtr markerId) override
    {
        m_pTraits->pushMarkerOverride(flag, markerId);
    }
    void                    popMarkerOverride(void) override { m_pTraits->popMarkerOverride(); }
    void                    clearMarkerOverride(void) override { m_pTraits->clearMarkerOverride(); }

private:
    AcGiSubEntityTraits* m_pTraits;

};


///////////////////////////////////////////////////////////////////////////////                                                       
//
// ** AuStWorldDraw **
class AuStWorldDraw : public AcGiWorldDraw
{
public:
    AuStWorldDraw(AcGiWorldDraw* pWD) : m_pWD(pWD), m_traits(&pWD->subEntityTraits()) { }

    AcGiWorldGeometry&  geometry() const override { return m_pWD->geometry(); }
    AcGiRegenType       regenType() const override { return m_pWD->regenType(); }
    Adesk::Boolean      regenAbort() const override { return m_pWD->regenAbort(); }
    AcGiGeometry*       rawGeometry() const override { return m_pWD->rawGeometry(); }
    Adesk::Boolean      isDragging() const override { return m_pWD->isDragging(); }
    Adesk::UInt32       numberOfIsolines() const override { return m_pWD->numberOfIsolines(); }

    AcGiContext*        context() override { return m_pWD->context(); }

    double deviation(const AcGiDeviationType dev, const AcGePoint3d& pt) const override
    {
        return m_pWD->deviation(dev, pt);
    }

    AcGiSubEntityTraits& subEntityTraits() const override
    {
        /* hook sub entity traits */
        return const_cast<AuStWorldDraw*>(this)->m_traits;
    }

private:
    AcGiWorldDraw* m_pWD;
    AuStSubEntityTraits m_traits;

};


///////////////////////////////////////////////////////////////////////////////                                                       
//
// ** AuStViewportDraw **
class AuStViewportDraw : public AcGiViewportDraw
{
public:
    AuStViewportDraw(AcGiViewportDraw* pVD) : m_pVD(pVD), m_traits(&m_pVD->subEntityTraits()) { }

    AcGiViewportGeometry& geometry() const override { return m_pVD->geometry(); }
    AcGiRegenType       regenType() const override { return m_pVD->regenType(); }
    Adesk::Boolean      regenAbort() const override { return m_pVD->regenAbort(); }
    AcGiGeometry* rawGeometry() const override { return m_pVD->rawGeometry(); }
    Adesk::Boolean      isDragging() const override { return m_pVD->isDragging(); }
    Adesk::UInt32       numberOfIsolines() const override { return m_pVD->numberOfIsolines(); }
    AcGiContext* context() override { return m_pVD->context(); }

    double deviation(const AcGiDeviationType dev, const AcGePoint3d& pt) const override
    {
        return m_pVD->deviation(dev, pt);
    }

    AcGiSubEntityTraits& subEntityTraits() const override
    {
        /* hook sub entity traits */
        return const_cast<AuStViewportDraw*>(this)->m_traits;
    }

private:
    AcGiViewportDraw* m_pVD;
    AuStSubEntityTraits m_traits;

};



///////////////////////////////////////////////////////////////////////////////                                                        
// Class Macros
//
// ** AuStEntityOverrule **
/* See SDK Documentation under "Class Implementation Macros" */
/* Or rxBoiler.h */
// ACRX_DEFINE_MEMBERS(AuStEntityOverrule); /* LINKER >> rxInit() not solved */
// ACRX_CONS_DEFINE_MEMBERS(AuStEntityOverrule, AcGiDrawableOverrule, 1); /* compile error >> build abstract class not possible */
ACRX_NO_CONS_DEFINE_MEMBERS(AuStEntityOverrule, AcGiDrawableOverrule); /* build ok */

                                                                       
///////////////////////////////////////////////////////////////////////////////                                                       
// Static Member
//
// ** AuStEntityOverrule **
AuStEntityOverrule *AuStEntityOverrule::_pEntityOverrule = nullptr;
////////#ifdef AUSTD
////////AcDbObjectId AuStEntityOverrule::m_LineTypeOid = AcDbObjectId::kNull;
////////#endif


///////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor
//
// ** AuStEntityOverrule **
AuStEntityOverrule::AuStEntityOverrule() 
{
    if (AuStEntityOverrule::desc() == nullptr) 
    {
        AuStEntityOverrule::rxInit();
        acrxBuildClassHierarchy();
    }
}
AuStEntityOverrule::~AuStEntityOverrule()
{
}


///////////////////////////////////////////////////////////////////////////////                                                       
// PUBLIC Class Methods
//
// ** AuStEntityOverrule **
void AuStEntityOverrule::Activate()
{    
    /* is already instantiated */
    if (_pEntityOverrule != nullptr)
        return;

    /* new overrule */
    _pEntityOverrule = new AuStEntityOverrule();

    /* add overrule */
    AcRxOverrule::addOverrule(
        AcDbEntity::desc(),
        _pEntityOverrule,
        true
    );

    /* enable overrule */
    _pEntityOverrule->Enable();

} // END
void AuStEntityOverrule::Deactivate(bool bUnregister /* = true */)
{
    if (_pEntityOverrule == nullptr)
        return;

    /* */
    /* Deactivate from AcRx::AppRetCode On_kUnloadAppMsg() */
    /* */

    /* disable overrule */
    _pEntityOverrule->Disable();

    if (bUnregister)
    {
        AcRxOverrule::removeOverrule(
            AcDbEntity::desc(),
            _pEntityOverrule
        );

        delete _pEntityOverrule;
        _pEntityOverrule = nullptr;
    } 

} // END
void AuStEntityOverrule::Enable() 
{ 
    m_bEnableOverrule = true; 

    if (!AcRxOverrule::isOverruling())
        AcRxOverrule::setIsOverruling(true);

    #ifdef AUSTD
    //acutPrintf(LFF _T("m_bEnableOverrule(%s)"), ASF, AuStUtil::BoolStr(m_bEnableOverrule).kACharPtr());
    //acutPrintf(LFF _T("isOverruling(%d)(%d)(%d)"), ASF, AcRxOverrule::isOverruling(), _pEntityOverrule->isOverruling(), AuStEntityOverrule::isOverruling());
    #endif
}
void AuStEntityOverrule::Disable() 
{
    m_bEnableOverrule = false;

    /* these have no effect! the flag will be true even when setting it to false... */
    /* This maybe come from the overrule not removed from AcRxOverrule::removeOverrule() */
    if (AcRxOverrule::isOverruling())
        AcRxOverrule::setIsOverruling(false);

    #ifdef AUSTD
    //acutPrintf(LFF _T("m_bEnableOverrule(%s)"), ASF, AuStUtil::BoolStr(m_bEnableOverrule).kACharPtr());
    //acutPrintf(LFF _T("isOverruling(%d)(%d)(%d)"), ASF, AcRxOverrule::isOverruling(), _pEntityOverrule->isOverruling(), AuStEntityOverrule::isOverruling());
    #endif
}


/////////////////////////////////////////////////
// PUBLIC Class Methods
//
// ** AuStEntityOverrule **
void AuStEntityOverrule::updateTraits(
    AcDb::LineWeight lwObject,
    AcCmTransparency &transp,
    AcDbObjectId& layerOid,
    Adesk::UInt16 &iEntityColor
)
{
    m_lwObject = lwObject;
    m_transp = transp;
    m_layerOid = layerOid;
    m_iEntityColor = iEntityColor;

    /* enable overrule >> need to disable overrule after BTR draw */
    Enable();

    #ifdef AUSTD
    /* here we create a new temporary test layer which we use to test the layer overrule */
    m_layerOid = AuStOLayer::Layer(_T("____TEMP_TEST_LAYER"), AcDb::kForRead);
    #endif

    #ifdef AUSTD
    iCounter = 0;
    #endif

} // END


/////////////////////////////////////////////////
// PRIVATE Class Methods
//
// ** AuStEntityOverrule **
bool AuStEntityOverrule::isApplicable(const AcRxObject *pOverruledSubject) const
{
    #ifdef AUSTD_pOff
    AcDbEntity *pEnt = AcDbEntity::cast(pOverruledSubject);
    acutPrintf(LFF _T("(%s) (_pEntityOverrule->isOverruling(%d)(%d) m_bEnableOverrule(%d)"), ASF,
        pEnt->isA()->name(),
        _pEntityOverrule->isOverruling(), AuStEntityOverrule::isOverruling(), m_bEnableOverrule);
    #endif

    /* here we need to implement our own ON/OFF flag to enable or disable the per object overrule */
    /* the IsOverruling() from the base class will be always 1, even if setIsOverruling(false) is applied */

    if (m_bEnableOverrule)
        return true;
    else
        return false;

} // END


/////////////////////////////////////////////////
// PRIVATE Class Methods
//
// ** AuStEntityOverrule **
Adesk::Boolean AuStEntityOverrule::worldDraw(AcGiDrawable* pSubject, AcGiWorldDraw* wd)
{
    /* skip entities for overrule */
    AcRxClass* rxClass = skipEntity(pSubject);
    if (rxClass != AcDbEntity::desc())
    {
        /* returning false for AcDbMText is neccessary for text and text background color */
        if (rxClass == AcDbMText::desc())
            return Adesk::kFalse;

        /* return base class world draw */
        else
            return AcGiDrawableOverrule::worldDraw(pSubject, &AuStWorldDraw(wd));
    }

    /* The default attributes are set, go ahead and override */
    setTraits(
        pSubject,
        wd,
        nullptr
    );

    /* return */
    return AcGiDrawableOverrule::worldDraw(pSubject, &AuStWorldDraw(wd));

} // END


/////////////////////////////////////////////////
// PRIVATE Class Methods
//
// ** AuStEntityOverrule **
void AuStEntityOverrule::viewportDraw(AcGiDrawable* pSubject, AcGiViewportDraw* vd)
{
    /* skip entities for overrule */
    if (AcDbEntity::desc() != skipEntity(pSubject))
        return AcGiDrawableOverrule::viewportDraw(pSubject, vd);

    /* The default attributes are set, go ahead and override */
    setTraits(
        pSubject,       
        nullptr,
        vd
    );

    /* return */
    return AcGiDrawableOverrule::viewportDraw(pSubject, vd);

} // END


/////////////////////////////////////////////////
// PRIVATE Class Methods
//
// ** AuStEntityOverrule **
void AuStEntityOverrule::setTraits(
    AcGiDrawable *pSubject,
    AcGiWorldDraw *wd,
    AcGiViewportDraw *vd
)
{
    /* get entity */
    AcDbEntity *pEnt = AcDbEntity::cast(pSubject);


    /* When the entity is part of a BTR, we check eByLayer and eByBlock */
    /* If the color of the entity is fixed (others than eByLayer || eByBlock), we DO NOT change the color! */


    if      (wd != nullptr)
        wd->subEntityTraits().setColor(m_iEntityColor);
    else if (vd != nullptr)
        vd->subEntityTraits().setColor(m_iEntityColor);


    /* set lineweight */
    if      (wd != nullptr)
        wd->subEntityTraits().setLineWeight(m_lwObject);
    else if (vd != nullptr)
        vd->subEntityTraits().setLineWeight(m_lwObject);


    /* set transparency */
    if      (wd != nullptr)
        wd->subEntityTraits().setTransparency(m_transp);
    else if (vd != nullptr)
        vd->subEntityTraits().setTransparency(m_transp);


    /* get layer from BTR entity when input is NULL >> is used for viewport display */
    if (m_layerOid == AcDbObjectId::kNull)
        m_layerOid = pEnt->layerId();

    /* otherwise, we use the BTR entity layer when layerOid is null */

    /* set layer */
    if      (wd != nullptr)
        wd->subEntityTraits().setLayer(m_layerOid);
    else if (vd != nullptr)
        vd->subEntityTraits().setLayer(m_layerOid);


    /* Housekeeping */
    pEnt = nullptr;


    /* return */
    return;

} // END


/////////////////////////////////////////////////
// PRIVATE Class Methods
//
// ** AuStEntityOverrule **
Adesk::UInt16 AuStEntityOverrule::getLayerColor(AcDbObjectId &oId)
{
    Adesk::UInt16 retValue = 7;
    AcDbLayerTableRecord* pLTR = nullptr;
    if (Acad::eOk == acdbOpenObject(
        pLTR,
        oId,
        AcDb::kForRead))
    {
        retValue = pLTR->color().colorIndex();
        pLTR->close();
    }
    pLTR = nullptr;
    return retValue;

} // END


/////////////////////////////////////////////////
// PRIVATE Class Methods
//
// ** AuStEntityOverrule **
AcRxClass* AuStEntityOverrule::skipEntity(AcGiDrawable* pSubject)
{
    /* skip AcDbAttributeDefinition */
    AcDbAttributeDefinition* pAtt = AcDbAttributeDefinition::cast(pSubject);
    if (pAtt != nullptr)
    {
        pAtt = nullptr;
        return AcDbAttributeDefinition::desc();
    }

    /* skip AcDbMText */
    AcDbMText* pMtxt = AcDbMText::cast(pSubject);
    if (pMtxt != nullptr)
    {
        pMtxt = nullptr;
        return AcDbMText::desc();
    }

    /* return */
    return AcDbEntity::desc();

} // END