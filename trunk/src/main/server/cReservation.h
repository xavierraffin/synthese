
#ifndef SYNTHESE_CRESERVATION_H
#define SYNTHESE_CRESERVATION_H

class cReservation;

#include "cModaliteReservation.h"
#include "cArretLogique.h"
#include "cAxe.h"
#include "cTexte.h"
#include "cTexteSQL.h"
#include "Temps.h"
#include "cPersonne.h"
#include "cProfil.h"
#include "cModaliteReservationEnLigne.h"

/** R�servation de transport � la demande
	@ingroup m37
*/
class cReservation
{
	//!	\name Service r�serv�
	//@{
	cAxe*							_AxeLigne;				//!< Axe de la ligne � la demande
	cTexte							_CodeService;			//!< Code du service au sein l'axe
	cArretLogique*							_ArretLogiqueMontee;		//!< Code du point d'arr�t de mont�e
	cArretLogique*							_ArretLogiqueDescente;	//!< Code du point d'arr�t de descente
	cModaliteReservationEnLigne*	_ModaliteReservation;	//!< Code de la modalit� de reservation
	cTexte							_AdresseDepart;			//!< Adresse du d�part
	cTexte							_AdresseArrivee;		//!< Adresse d'arriv�e
	cMoment							_MomentDepart;			//!< Moment de mont�e pr�vu
	cMoment							_MomentArrivee;			//!< Moment d'arriv�e pr�vue
	//@}
	
	//!	\name Caract�ristiques de la r�servation
	//@{
	cTexte							_Code;					//!< Code � 7 lettres d'identification unique
	cTexte							_CodePredecesseur;		//!< Code de la r�servation annul�e en cas de modification
	size_t							_NombrePlaces;			//!< Nombre de places
	cMoment							_MomentReservation;		//!< Date de la r�servation
	bool							_Annulee;				//!< R�servation annul�e?
	cMoment							_MomentAnnulation;		//!< Date de l'annulation
	//@}

	//!	\name Personnes
	//@{
	cPersonne*						_Personne;
	cProfil*						_PersonneSaisie;
	cProfil*						_PersonneAnnulation;
	//@}

public:
	//!	\name Modificateurs
	//@{
	void	SetValeurs(const cTrain* Service	, const cArretLogique* tPADepart, const cArretLogique* tPAArrivee
						, const cMoment& DateDepart, const cTexteSQL& tNom, const cTexteSQL& tNomBrut
						, const cTexteSQL& tPrenom, const cTexteSQL& tAdresse, const cTexteSQL& tEmail
						, const cTexteSQL& tTelephone, const cTexteSQL& tNumAbonne
						, const cTexteSQL& tAdressePAArrivee, const cTexteSQL& tAdressePADepart
						, const size_t iNombrePlaces);
	void	SetCode(const cTexte& __Code);
	//@}
	
	//!	\name Modificateurs
	//@{
	bool	Reserver();
	bool	Annuler();
	bool	Charger();
	//@}
	
	//!	\name Accesseurs
	//@{
	const cMoment&		MomentDepart()		const;
	const cMoment&		MomentArrivee()		const;
	const cPersonne*	GetPersonne()		const;
	bool				GetAnnulee()		const;
	//@}
	
	cReservation();
	~cReservation();
};

#endif

