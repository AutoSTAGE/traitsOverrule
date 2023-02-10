#ifndef _AUSTOLAYER_H
#define _AUSTOLAYER_H

//-----------------------------------------------------------------------------
//----- Declaration of AuStOLayer
//-----------------------------------------------------------------------------
#include "AuStLayerProperty.h"

class AuStOLayer
{


public:
	/////////////////////////////////////////////////
	// Static Public Functions
	//
	// ** AuStOLayer **
	#pragma region // Static Public Functions //

	static Acad::ErrorStatus Layer(		/*	DESC	>>	Gets layer with name;
														Returns current layer when name is nullptr or EMPTY;
														Create layer if not found in layertable */
		AcString *astrLayerName,		/*	INPUT	>>	Name of layer, return current layer when nullptr or EMPTY */
		AcDbObjectId &oIdLayer,			/*	RETURN	>>	ObjectId of layer */
		AuStLayerProperty *layProp,		/*	Layer Properties */
		AcDb::OpenMode openMode
			= AcDb::kForRead			/*	PARAM	>> read only (on/off, freeze and lock) for existing layers */
	);

	static AcDbObjectId Layer(			/* READ only >> Returns ObjectId of Layer */
		AcString astrLayerName,
		AcDb::OpenMode openMode
		= AcDb::kForNotify				/* AcDb::kForNotify >> gets objectId when layer is there, otherwise return kNull */
	);


	static const AcDbObjectId GetClayer();


	static void AuStOLayer::ShowAndUnlockClayer();


	static Acad::ErrorStatus CreateLayer(		/* DESC		>>	Creates a new layer */
		AcString *astrLayerName,				/* INPUT	>>	Name of new layer */
		AcDbObjectId &oIdLayer,					/* RETURN	>>	ObjectId of new layer */
		AuStLayerProperty *layProp = nullptr,	/* Layer Properties */
		AcDbLayerTable *pLT = nullptr			/* PARAM	>>	Optional */
	);


	static Acad::ErrorStatus FindLayer(	/*	DESC	>>	Find layer with objectId */
		AcDbObjectId &oIdLayer,			/*  INPUT	>>	ObjectId of layer to find */
										/*				(1)     When this is not kNull, we search for the layer with the objectId */
										/*				(2)     When this is kNull, this is the RETURN value */
										/*				(***)   This takes precedence over the layer name */			
		AcString *astrLayerName,		/*  RETURN	>>	Name of found layer */
										/*				(1)     When this is not EMPTY and(!!) the objectId is kNull, we search for the layer with this name */
										/*				(2)     When this is EMPTY, this is the RETURN value */		
		AuStLayerProperty *layProp		/* Layer Properties */
			= nullptr,
		AcDb::OpenMode openMode
			= AcDb::kForRead,				/*	PARAM	>>	Read only (on/off, freeze and lock) for existing layers */		
		AcDbLayerTable *pLT = nullptr,	/*	PARAM	>>	Optional */
		bool *bDeleteLayer = nullptr	/*	PARAM	>>	Optional, deletes this layer if the layer is empty */
	);





	#pragma endregion

};

#endif // _AuStOLayer_H
