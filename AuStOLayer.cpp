#include "acgi.h"
#include "dbmain.h"
#include "dbents.h"
#include "drawable.h"
#include "dbmtext.h"
#include "tchar.h"
#include "AuStOLayer.h"
#include "AuStLayerProperty.h"
#include "dbapserv.h"

//#include "arxHeaders.h"


#ifdef AUSTD
#define AUSTDEBUG_DEEPCLONE
#endif


/////////////////////////////////////////////////
// Static Public Function
//
// ** AuStOLayer **
Acad::ErrorStatus AuStOLayer::Layer(
    AcString *astrLayerName,
    AcDbObjectId &oIdLayer,
    AuStLayerProperty *layProp,	/*	Layer Properties */
    AcDb::OpenMode openMode /* = AcDb::kForRead */
)
{
    /* Error status */
    Acad::ErrorStatus   asErr = Acad::eNotApplicable;


    #pragma region // Get Name of current Layer when LayerName is nullptr or EMPTY //

    if ((astrLayerName == nullptr) ||
        (*astrLayerName == _T("")))
    {
        /* get current layer */
        oIdLayer = AuStOLayer::GetClayer();

        /* return */
        if (oIdLayer == AcDbObjectId::kNull)
            return Acad::eNullObjectId;
    }

    #pragma endregion


    #pragma region // Get layer table //

    AcDbLayerTable *pLT = nullptr;
    if (Acad::eOk != (asErr = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLT, AcDb::kForRead)))
    {
        return asErr;
    }

    #pragma endregion


    #pragma region // Find named layer, create or iterate layer //

    /* error cycle */
    do
    {
        /* get layer with name >> create when not found >> returns objectId */
        if ((astrLayerName != nullptr) &&
            (*astrLayerName != _T("")) &&
            (Acad::eKeyNotFound == (asErr = pLT->getAt(astrLayerName->kACharPtr(), oIdLayer, Adesk::kFalse))) &&
            (openMode != AcDb::kForNotify)      /* do not create new layer when openMode is NOT kForRead of kForWrite */
            )
        {
            if (Acad::eOk != (asErr = AuStOLayer::CreateLayer(astrLayerName, oIdLayer, layProp, pLT)))
                break; /* error */
        }

        /* got objectId >> get/set layer states */
        if (oIdLayer != AcDbObjectId::kNull)
        {
            if (Acad::eOk != (asErr = AuStOLayer::FindLayer(oIdLayer, astrLayerName, layProp, openMode, pLT)))
                break; /* error */
        }

        /* done error */
        break;

    } while (true);

    #pragma endregion


    /* Housekeeping */
    pLT->close(); pLT = nullptr;

    /* return */
    return (asErr);

} // END

/* READ only >> Returns ObjectId of Layer */
AcDbObjectId AuStOLayer::Layer(
    AcString astrLayerName,
    AcDb::OpenMode openMode /* = AcDb::kForNotify */
)
{
    /* Error status */
    Acad::ErrorStatus   asErr = Acad::eNotApplicable;

    /* return value */
    AcDbObjectId oIdLayer = AcDbObjectId::kNull;

    /* Properties */
    std::unique_ptr<AuStLayerProperty> layProp(new AuStLayerProperty());

    /* AcDb::kForNotify >> gets objectId when layer is there, otherwise return kNull */
    AuStOLayer::Layer(&astrLayerName, oIdLayer, layProp.get(), openMode);

    /* return */
    return oIdLayer;

} // END






/////////////////////////////////////////////////
// Static Public Function
//
// ** AuStOLayer **
const AcDbObjectId AuStOLayer::GetClayer()
{
    #ifdef AUSTD_off
    acutPrintf(LFF LIT("Current Layer is (%lx)"), ASF, acdbHostApplicationServices()->workingDatabase()->clayer());
    #endif
    return acdbHostApplicationServices()->workingDatabase()->clayer();

} // END

void AuStOLayer::ShowAndUnlockClayer()
{
    AcDbObjectId oIdLayer = AcDbObjectId::kNull;
    std::unique_ptr<AuStLayerProperty> layProp(new AuStLayerProperty());
    layProp->setIsFrozen(false);
    layProp->setIsOff(false);
    layProp->setIsLocked(false);
    AuStOLayer::Layer(NULL, oIdLayer, layProp.get(), AcDb::kForWrite);

} // END


/////////////////////////////////////////////////
// Static Public Function
//
// ** AuStOLayer **
Acad::ErrorStatus AuStOLayer::CreateLayer(
    AcString *astrLayerName,
    AcDbObjectId &oIdLayer,
    AuStLayerProperty *layProp  /* = nullptr */,
    AcDbLayerTable *pLT         /* = nullptr */
)
{
    /* ErrorStatus */
    Acad::ErrorStatus asErr = Acad::eNotApplicable;


    #pragma region // Get layer table //

    bool bLtHasBeenNull = false;
    if (pLT == nullptr)
    {
        bLtHasBeenNull = true;
        if (Acad::eOk != (asErr = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLT, AcDb::kForRead)))
        {
            return asErr;
        }
    }

    #pragma endregion


    #pragma region // New LTR //

    /* error cycle */
    do
    {
        /* Create a new layer table record using the layer name passed in */
        AcDbLayerTableRecord *pLTR = new AcDbLayerTableRecord;
        if (pLTR == nullptr)
        {
            asErr = Acad::eNullObjectPointer;
            break; /* error */
        }

        /* Set Layer Name */
        asErr = pLTR->setName(astrLayerName->kACharPtr());

        /* set state */
        if (layProp != nullptr)
        {
            pLTR->setIsHidden(layProp->IsHidden());
            pLTR->setIsFrozen(layProp->IsFrozen());
            pLTR->setIsOff(layProp->IsOff());
            pLTR->setIsLocked(layProp->IsLocked());
            pLTR->setIsPlottable(layProp->PrintEnabled());
            pLTR->setColor(layProp->Color());
        }

        /* Add to LayerTable */
        if (Acad::eOk == (asErr = pLT->upgradeOpen()))
        {
            /* add layer */
            asErr = pLT->add(oIdLayer, pLTR);

            /* Since the new layer was successfully added to the database, */
            /* close it - DON'T delete it */
            asErr = pLTR->close();
        }
        else
        {
            /* The object is not part of the database, therefore we should */
            /* delete it to avoid a memory leak */
            delete pLTR;
        }

        /* housekeeping */
        pLTR = nullptr;

        #ifdef AUSTD_off
        if (asErr == Acad::eOk)
            acutPrintf(LFF LIT("Created New Layer(%lx)(%s)"), ASF, oIdLayer, astrLayerName->kACharPtr());
        #endif

        /* done error cycle */
        break;

    } while (true);

    #pragma endregion


    /* housekeeping */
    if (bLtHasBeenNull)
    {
        asErr = pLT->close();
        pLT = nullptr;
    }

    /* Error */
    //if (asErr != Acad::eOk)
    //    acutPrintf(LFF LIT("Error(%s) to created New Layer(%s)"), ASF, acadErrorStatusText(asErr), astrLayerName->kACharPtr());

    /* return */
    return asErr;

} // END


/////////////////////////////////////////////////
// Static Public Function
//
// ** AuStOLayer **
Acad::ErrorStatus AuStOLayer::FindLayer(

    /*  INPUT >> objectId of layer to find */
    /*      (1)     When this is not kNull, we search for the layer with the objectId */
    /*      (2)     When this is kNull, this is the RETURN value */
    /*      (***)   This takes precedence over the layer name */
    AcDbObjectId &oIdLayer,

    /*  RETURN >> name of found layer */
    /*      (1)     When this is not EMPTY and(!!) the objectId is kNull, we search for the layer with this name */
    /*      (2)     When this is EMPTY, this is the RETURN value */
    AcString *astrLayerName,

    AuStLayerProperty *layProp,	/* Layer Properties */
    AcDb::OpenMode openMode,	/* = AcDb::kForRead */
    AcDbLayerTable *pLT,		/* = nullptr */
    bool *bDeleteLayer          /* = nullptr */
)
{
    /* ErrorStatus */
    Acad::ErrorStatus asErr = Acad::eNotApplicable;


    #pragma region // Get layer table //

    bool bGotLayerTablePointer = false;
    if (pLT == nullptr)
    {
        bGotLayerTablePointer = true;
        if (Acad::eOk != (asErr = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLT, AcDb::kForRead)))
        {
            return asErr;
        }
    }

    #pragma endregion


    #pragma region // Iterate Layer //

    /* error cycle */
    do
    {
        /* new layer Iterator */
        AcDbLayerTableIterator *pLayerTableIter = nullptr;
        if (Acad::eOk != (asErr = pLT->newIterator(pLayerTableIter)))
        {
            if (pLayerTableIter) delete pLayerTableIter; pLayerTableIter = nullptr;
            break; /* error */
        }

        /* skip hidden layers */
        pLayerTableIter->setSkipHidden(layProp->SkipHidden());

        /* Iterate LayerTable */
        for (; !pLayerTableIter->done(); pLayerTableIter->step())
        {
            /* new LayerTableRecord */
            AcDbLayerTableRecord *pLTR = nullptr;
            if (Acad::eOk != (asErr = (pLayerTableIter->getRecord(pLTR, AcDb::kForRead))))
            {
                continue;
            }
            #ifdef AUSTD_pOff
            else
                acutPrintf(LFF _T("pLTR->objectId(%lx)"), ASF, pLTR->objectId());
            #endif

            /* break flag */
            bool bBreakIter = false;

            /* compare condition */
            bool bFoundLayer = false;

            /* by objectId */
            if (oIdLayer != AcDbObjectId::kNull)
            {
                if (oIdLayer == pLTR->objectId())
                    bFoundLayer = true;
            }

            /* by name */
            else if ((astrLayerName != nullptr) &&
                    (!astrLayerName->isEmpty()) &&
                    (oIdLayer == AcDbObjectId::kNull))
            {
                /* get name */
                AcString tmp;
                pLTR->getName(tmp);

                /* compare with name */
                if (tmp == *astrLayerName)
                    bFoundLayer = true;
            }

            /* found layer by condition */
            if (bFoundLayer)
            {
                /* delete layer */
                if ((bDeleteLayer != nullptr) &&
                    (*bDeleteLayer == true) &&
                    (pLTR->upgradeOpen() == Acad::eOk))
                {
                    pLTR->erase();

                    /* Housekeeping */
                    if (pLTR) pLTR->close();
                    pLTR = nullptr;

                    /* done >> break iter */
                    break;
                }

                /* by objectId */
                if (oIdLayer != AcDbObjectId::kNull)
                {
                    /* get Layer Name */
                    if (astrLayerName != nullptr)
                    {
                        pLTR->getName(*astrLayerName);
                    }
                }

                /* by name */
                else if ((astrLayerName != nullptr) &&
                        (!astrLayerName->isEmpty()) &&
                        (oIdLayer == AcDbObjectId::kNull))
                {
                    oIdLayer = pLTR->objectId();
                }

                /* layer properties */
                layProp->EditProperties(pLTR, openMode);

                /* done >> break */
                bBreakIter = true;
            }

            /* Housekeeping */
            if (pLTR) pLTR->close();
            pLTR = nullptr;

            /* break Iterator */
            if (bBreakIter)
                break;

        } // for pLayerTableIter

        /* Housekeeping */
        delete pLayerTableIter;
        pLayerTableIter = nullptr;

        #ifdef AUSTD_off
        if (asErr == Acad::eOk)
            acutPrintf(LFF _T("Found Layer(%lx)(%s)"), ASF, oIdLayer, astrLayerName->kACharPtr());
        #endif

        /* done error cycle */
        break;

    } while (true);

    #pragma endregion


    /* housekeeping */
    if (bGotLayerTablePointer)
    {
        asErr = pLT->close();
        pLT = nullptr;
    }

    /* Error */
    //if ((asErr != Acad::eOk) &&
    //    (astrLayerName != nullptr))
    //    acutPrintf(LFF _T("Error(%s) to find layer by name(%s)"), ASF, acadErrorStatusText(asErr), astrLayerName->kACharPtr());

    /* return */
    return asErr;

} // END



