
/** DeparturesTableModule class header.
	@file DeparturesTableModule.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_DeparturesTableModule_H__
#define SYNTHESE_DeparturesTableModule_H__

#include <map>
#include <string>

#include "01_util/ModuleClass.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/DisplayScreen.h"

/** @defgroup m34 34 Arrival/departures tables service module.

	@subsection m34_target_structure Architecture fonctionnelle cible

	Un syst�me de t�l�affichage bas� sur SYNTHESE repose sur les principes suivants :
		- la base de donn�es de SYNTHESE constitue le point de convergence des diverses sources de donn�es. Dans le cadre du t�l�affichage, les sources majeures sont : 
		- les sources dites <i>th�oriques</i> : elles proviennent de fichiers mis � jour par saisie manuelle ou par import informatique
		- les sources dites <i>temps r�el</i> : saisies manuelles dans la console d'administration (ex : saisie de messages...) ou r�ceptions de donn�es transmises automatiquement (ex : contr�les de supervision issues du client de supervision, import modifications horaires SAE...)
		- le t�l�affichage est un service du syst�me SYNTHESE comme un autre : il exploite la base de donn�es � chaud et fabrique des objets r�sultats (une liste de d�parts ou d'arriv�e) qui sont transmis au module d'interface, charg� de les diffuser selon un langage d�fini
		- le module d'interface permet de d�finir le format des sorties sous forme de donn�es d'entr�e, dans une grande libert� de choix. Les formats de sortie peuvent �tre d�finis en HTML pour int�gration � une charte graphique, en XML pour communiquer avec d'autres applications, ou encore selon un langage propri�taire pour piloter des dispositifs de t�l�affichage mat�riels

	Le sch�ma suivant repr�sente l'architecture cible d'un syst�me de t�l�affichage bas� sur la solution SYNTHESE :

	@image latex 1195515_1_000.eps "Architecture cible d'un syst�me de t�l�affichage SYNTHESE" height=10cm
	@image html 1195515.2.000.png

@{
*/

namespace synthese
{
	namespace env
	{
		class LineStop;
		class ConnectionPlace;
		class PhysicalStop;
		class Line;
	}
	namespace departurestable
	{
		class DeparturesTableModule : public util::ModuleClass
		{
			static DisplayType::Registry		_displayTypes;
			static BroadcastPoint::Registry		_broadcastPoints;
			static DisplayScreen::Registry		_displayScreens;

		public:
			static DisplayType::Registry&		getDisplayTypes();
			static BroadcastPoint::Registry&	getBroadcastPoints();
			static DisplayScreen::Registry&		getDisplayScreens();

			static std::map<uid, std::string>	getDisplayTypeLabels(bool withAll = false);
			static std::map<uid, std::string>	getPlacesWithBroadcastPointsLabels(bool withAll = false);
			static std::map<uid, std::string>	getBroadcastPointLabels(const env::ConnectionPlace* place, bool withAll = false);
		};
	}
}

/** @} */

#endif // SYNTHESE_DeparturesTableModule_H__
