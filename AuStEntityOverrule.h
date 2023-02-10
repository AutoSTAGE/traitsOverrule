
//-----------------------------------------------------------------------------
//----- AuStEntityOverrule.h : Declaration of the AuStEntityOverrule
//-----------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------
class AuStEntityOverrule : public AcGiDrawableOverrule
{
public:
    ACRX_DECLARE_MEMBERS(AuStEntityOverrule);


private:
    AcDb::LineWeight m_lwObject;
    AcCmTransparency m_transp;
    AcDbObjectId m_layerOid;
    Adesk::UInt16 m_iEntityColor;
    bool m_bEnableOverrule;
    #ifdef AUSTD
    int iCounter;
    //static AcDbObjectId m_LineTypeOid;
    #endif
    
public:    
    static AuStEntityOverrule *_pEntityOverrule;


public:
    /////////////////////////////////////////////////
    // Constructor, Destructor
    //
    // ** AuStEntity **
    AuStEntityOverrule();
    ~AuStEntityOverrule();


public:
    /////////////////////////////////////////////////
    // PUBLIC Class Methods
    //
    // ** AuStEntityOverrule **
    static void Activate();
    static void Deactivate(bool bUnregister = false);

    void Enable();
    void Disable();


    /////////////////////////////////////////////////
    // PUBLIC Class Methods
    //
    // ** AuStEntityOverrule **
    void updateTraits(
        AcDb::LineWeight lwObject,
        AcCmTransparency &transp,
        AcDbObjectId& layerOid,
        Adesk::UInt16& iEntityColor
    );


private:
    /////////////////////////////////////////////////
    // PRIVATE Class Methods
    //
    // ** AuStEntityOverrule **
    virtual bool            isApplicable(const AcRxObject *pOverruledSubject = nullptr) const ADESK_OVERRIDE;
    virtual Adesk::Boolean  worldDraw(AcGiDrawable *pSubject, AcGiWorldDraw *wd) ADESK_OVERRIDE;
    virtual void            viewportDraw(AcGiDrawable *pSubject, AcGiViewportDraw *vd) ADESK_OVERRIDE;

    void setTraits(
        AcGiDrawable *pSubject,
        AcGiWorldDraw *wd,
        AcGiViewportDraw *vd
    );
    Adesk::UInt16 getLayerColor(AcDbObjectId& oId);
    AcRxClass* AuStEntityOverrule::skipEntity(AcGiDrawable* pSubject);
};
