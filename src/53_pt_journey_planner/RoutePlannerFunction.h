////////////////////////////////////////////////////////////////////////////////
/// RoutePlannerFunction class header.
///	@file RoutePlannerFunction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_RoutePlannerFunction_H__
#define SYNTHESE_RoutePlannerFunction_H__

#include "FactorableTemplate.h"
#include "FunctionWithSite.h"

#include "AccessParameters.h"
#include "AlgorithmTypes.h"
#include "PTRoutePlannerResult.h"
#include "TransportWebsite.h"

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

namespace synthese
{
	class CoordinatesSystem;

	namespace algorithm
	{
		class AlgorithmLogger;
	}

	namespace road
	{
		class Crossing;
		class RoadPlace;
		class Road;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace pt_website
	{
		class HourPeriod;
		class RollingStockFilter;
	}

	namespace geography
	{
		class Place;
		class NamedPlace;
	}

	namespace pt
	{
		class StopPoint;
		class StopArea;
		class ReservationContact;
		class StopPoint;
	}

	namespace server
	{
		class Request;
	}

	namespace graph
	{
		class ServicePointer;
		class Journey;
		class Vertex;
	}

	namespace pt_journey_planner
	{
		class RoutePlannerInterfacePage;
		class UserFavoriteJourney;

		////////////////////////////////////////////////////////////////////
		/// 53.15 Function : public transportation route planner.
		///	@ingroup m53Functions refFunctions
		/// @author Hugues Romain
		///
		/// Usage : https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner
		///
		class RoutePlannerFunction:
			public util::FactorableTemplate<cms::FunctionWithSite<false>, RoutePlannerFunction>
		{
		public:
			static const std::string PARAMETER_SITE;
			static const std::string PARAMETER_MAX_SOLUTIONS_NUMBER;
			static const std::string PARAMETER_APPROACH_SPEED;
			static const std::string PARAMETER_MAX_DEPTH;
			static const std::string PARAMETER_DEPARTURE_CITY_TEXT;
			static const std::string PARAMETER_ARRIVAL_CITY_TEXT;
			static const std::string PARAMETER_DEPARTURE_PLACE_TEXT;
			static const std::string PARAMETER_ARRIVAL_PLACE_TEXT;
			static const std::string PARAMETER_DEPARTURE_CLASS_FILTER;
			static const std::string PARAMETER_ARRIVAL_CLASS_FILTER;
			static const std::string PARAMETER_ACCESSIBILITY;
			static const std::string PARAMETER_FAVORITE_ID;
			static const std::string PARAMETER_DAY;
			static const std::string PARAMETER_PERIOD_ID;
			static const std::string PARAMETER_LOWEST_DEPARTURE_TIME;
			static const std::string PARAMETER_LOWEST_ARRIVAL_TIME;
			static const std::string PARAMETER_HIGHEST_DEPARTURE_TIME;
			static const std::string PARAMETER_HIGHEST_ARRIVAL_TIME;
			static const std::string PARAMETER_ROLLING_STOCK_FILTER_ID;
			static const std::string PARAMETER_LOG_PATH;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_SHOW_COORDINATES;
			static const std::string PARAMETER_MAX_TRANSFER_DURATION;
			static const std::string PARAMETER_MIN_MAX_DURATION_RATIO_FILTER;
			static const std::string PARAMETER_MIN_WAITING_TIME_FILTER;
			static const std::string PARAMETER_SIMILAR_TIME_DELAY;
			static const std::string PARAMETER_DURATION_RATIO_SIMILAR_TIME_FILTER;
			static const std::string PARAMETER_FARE_CALCULATION;

			static const std::string PARAMETER_OUTPUT_FORMAT;
			static const std::string VALUE_ADMIN_HTML;

			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_SCHEDULES_ROW_PAGE;
			static const std::string PARAMETER_SCHEDULES_CELL_PAGE;
			static const std::string PARAMETER_LINES_ROW_PAGE;
			static const std::string PARAMETER_LINE_MARKER_PAGE;
			static const std::string PARAMETER_BOARD_PAGE;
			static const std::string PARAMETER_WARNING_PAGE;
			static const std::string PARAMETER_WARNING_CHECK_PAGE;
			static const std::string PARAMETER_RESERVATION_PAGE;
			static const std::string PARAMETER_DURATION_PAGE;
			static const std::string PARAMETER_TEXT_DURATION_PAGE;
			static const std::string PARAMETER_MAP_PAGE;
			static const std::string PARAMETER_MAP_LINE_PAGE;
			static const std::string PARAMETER_DATE_TIME_PAGE;
			static const std::string PARAMETER_STOP_CELL_PAGE;
			static const std::string PARAMETER_SERVICE_CELL_PAGE;
			static const std::string PARAMETER_JUNCTION_CELL_PAGE;
			static const std::string PARAMETER_TICKET_CELL_PAGE;
			static const std::string PARAMETER_MAP_STOP_PAGE;
			static const std::string PARAMETER_MAP_SERVICE_PAGE;
			static const std::string PARAMETER_MAP_JUNCTION_PAGE;
			static const std::string PARAMETER_SHOW_RESULT_TABLE;
			static const std::string PARAMETER_RESULT_ROW_PAGE;

		private:
			static const std::string DATA_LINES;
			static const std::string DATA_SCHEDULES;
			static const std::string DATA_WARNINGS;
			static const std::string DATA_DURATIONS;
			static const std::string DATA_RESERVATIONS;
			static const std::string DATA_BOARDS;
			static const std::string DATA_SOLUTIONS_NUMBER;
			static const std::string DATA_MAPS_LINES;
			static const std::string DATA_MAPS;
			static const std::string DATA_FILTERED_JOURNEYS;
			static const std::string DATA_MAX_WARNING_LEVEL_ON_STOP;
			static const std::string DATA_MAX_WARNING_LEVEL_ON_LINE;
			static const std::string DATA_HAS_RESERVATION;
			static const std::string DATA_RESULT_ROWS;

			//! @name Cells
			//@{
			static const std::string DATA_COLUMN_NUMBER;
			static const std::string DATA_CONTENT;
			//@}

			static const std::string DATA_IS_FIRST_ROW;
			static const std::string DATA_IS_LAST_ROW;
			static const std::string DATA_IS_FOOT;
			static const std::string DATA_FIRST_TIME;
			static const std::string DATA_LAST_TIME;
			static const std::string DATA_IS_CONTINUOUS_SERVICE;
			static const std::string DATA_IS_FIRST_WRITING;
			static const std::string DATA_IS_LAST_WRITING;
			static const std::string DATA_IS_FIRST_FOOT;

			//! @name Rows
			//@{
			static const std::string DATA_CELLS;
			static const std::string DATA_IS_ODD_ROW;
			static const std::string DATA_IS_ORIGIN_ROW;
			static const std::string DATA_IS_DESTINATION_ROW;
			static const std::string DATA_PLACE_NAME;
			//@}

			static const std::string DATA_LINE_ID;
			static const std::string DATA_STOP_ID;
			static const std::string DATA_INTERNAL_DATE;
			static const std::string DATA_ORIGIN_CITY_TEXT;
			static const std::string DATA_HANDICAPPED_FILTER;
			static const std::string DATA_ORIGIN_PLACE_TEXT;
			static const std::string DATA_BIKE_FILTER;
			static const std::string DATA_DESTINATION_CITY_TEXT;
			static const std::string DATA_DESTINATION_PLACE_ID;
			static const std::string DATA_DESTINATION_PLACE_TEXT;
			static const std::string DATA_PERIOD_ID;
			static const std::string DATA_DATE;
			static const std::string DATA_PERIOD;
			static const std::string DATA_ACCESSIBILITY_CODE;
			static const std::string DATA_SITE_ID;
			static const std::string DATA_IS_SAME_PLACES;
			static const std::string DATA_USER_FULL_NAME;
			static const std::string DATA_USER_PHONE;
			static const std::string DATA_USER_ID;
			static const std::string DATA_RANK;
				static const std::string DATA_DEPARTURE_TIME;
				static const std::string DATA_DEPARTURE_PLACE_NAME;
				static const std::string DATA_ARRIVAL_TIME;
				static const std::string DATA_ARRIVAL_PLACE_NAME;
				static const std::string DATA_DURATION;
				static const std::string DATA_DEPARTURE_DATE;
				static const std::string DATA_RESERVATION_AVAILABLE;
				static const std::string DATA_RESERVATION_COMPULSORY;
				static const std::string DATA_RESERVATION_DELAY;
				static const std::string DATA_RESERVATION_DEADLINE;
				static const std::string DATA_RESERVATION_PHONE_NUMBER;
				static const std::string DATA_ONLINE_RESERVATION;
				static const std::string DATA_DEPARTURE_TIME_INTERNAL_FORMAT;
				static const std::string DATA_IS_THE_LAST_JOURNEY_BOARD;
				static const std::string DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME;
				static const std::string DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME;
				static const std::string DATA_DEPARTURE_PLACE_LONGITUDE;
				static const std::string DATA_DEPARTURE_PLACE_LATITUDE;
				static const std::string DATA_ARRIVAL_PLACE_LONGITUDE;
				static const std::string DATA_ARRIVAL_PLACE_LATITUDE;
				static const std::string DATA_CO2_EMISSIONS;
				static const std::string DATA_ENERGY_CONSUMPTION;
				static const std::string DATA_TICKETS;
				static const std::string DATA_TICKET_NAME;
				static const std::string DATA_TICKET_PRICE;
				static const std::string DATA_TICKET_CURRENCY;
				static const std::string DATA_START_DATE;
				static const std::string DATA_END_DATE;
			//@}

			//! @name Cells
			//@{
				static const std::string DATA_ODD_ROW;
				static const std::string DATA_LONGITUDE;
				static const std::string DATA_LATITUDE;
			//@}

			//! @name Stop cells
			//@{
				static const std::string DATA_IS_ARRIVAL;
				static const std::string DATA_IS_TERMINUS;
				static const std::string DATA_STOP_AREA_NAME;
				static const std::string DATA_DEPARTURE_STOP_NAME;
				static const std::string DATA_DEPARTURE_LONGITUDE;
				static const std::string DATA_DEPARTURE_LATITUDE;
				static const std::string DATA_ARRIVAL_STOP_NAME;
				static const std::string DATA_ARRIVAL_LONGITUDE;
				static const std::string DATA_ARRIVAL_LATITUDE;
				static const std::string DATA_IS_LAST_LEG;
				static const std::string DATA_IS_FIRST_LEG;
			//@}

			//! @name Junction cells
			//@{
				static const std::string DATA_REACHED_PLACE_IS_NAMED;
				static const std::string DATA_ROAD_NAME;
				static const std::string DATA_LENGTH;
			//@}

			//! @name Service cells
			//@{
				static const std::string DATA_FIRST_DEPARTURE_TIME;
				static const std::string DATA_LAST_DEPARTURE_TIME;
				static const std::string DATA_FIRST_ARRIVAL_TIME;
				static const std::string DATA_LAST_ARRIVAL_TIME;
				static const std::string DATA_ROLLINGSTOCK_ID;
				static const std::string DATA_ROLLINGSTOCK_NAME;
				static const std::string DATA_ROLLINGSTOCK_ARTICLE;
				static const std::string DATA_DESTINATION_NAME;
				static const std::string DATA_HANDICAPPED_FILTER_STATUS;
				static const std::string DATA_HANDICAPPED_PLACES_NUMBER;
				static const std::string DATA_BIKE_FILTER_STATUS;
				static const std::string DATA_BIKE_PLACES_NUMBER;
				static const std::string DATA_CONTINUOUS_SERVICE_WAITING;
				static const std::string DATA_WKT;
			//@}

			//! @name Result row
			//@{
				static const std::string DATA_LINE_MARKERS;
				static const std::string DATA_ROW_NUMBER;
			//@}

			typedef std::vector<boost::shared_ptr<std::ostringstream> > PlacesContentVector;

		private:
			//! \name Parameters
			//@{
				road::RoadModule::ExtendedFetchPlaceResult	_departure_place;
				road::RoadModule::ExtendedFetchPlaceResult	_arrival_place;
				std::string									_originCityText;
				std::string									_destinationCityText;
				std::string									_originPlaceText;
				std::string									_destinationPlaceText;
				boost::gregorian::date						_day;
				std::size_t									_periodId;
				boost::posix_time::ptime					_startDate;
				boost::posix_time::ptime					_endDate;
				boost::posix_time::ptime					_startArrivalDate;
				boost::posix_time::ptime					_endArrivalDate;
				graph::AccessParameters						_accessParameters;
				boost::optional<std::size_t>				_maxSolutionsNumber;
				const pt_website::HourPeriod*			_period;
				boost::shared_ptr<const UserFavoriteJourney>		_favorite;
				boost::shared_ptr<const pt_website::RollingStockFilter>	_rollingStockFilter;
				bool										_outputRoadApproachDetail;
				bool _showResTab;
				bool _showCoords;
				const CoordinatesSystem* _coordinatesSystem;
				boost::optional<boost::posix_time::time_duration> _maxTransferDuration;
				boost::optional<double> _minMaxDurationRatioFilter;
				boost::optional<boost::posix_time::time_duration> _minWaitingTimeFilter;
				bool _fareCalculation;
				boost::shared_ptr<algorithm::AlgorithmLogger> _logger;
				std::string									_outputFormat;
			//@}

			//! @name Pages
			//@{
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _schedulesRowPage;
				boost::shared_ptr<const cms::Webpage> _schedulesCellPage;
				boost::shared_ptr<const cms::Webpage> _linesRowPage;
				boost::shared_ptr<const cms::Webpage> _lineMarkerPage;
				boost::shared_ptr<const cms::Webpage> _boardPage;
				boost::shared_ptr<const cms::Webpage> _warningPage;
				boost::shared_ptr<const cms::Webpage> _warningCheckPage;
				boost::shared_ptr<const cms::Webpage> _reservationPage;
				boost::shared_ptr<const cms::Webpage> _durationPage;
				boost::shared_ptr<const cms::Webpage> _textDurationPage;
				boost::shared_ptr<const cms::Webpage> _mapPage;
				boost::shared_ptr<const cms::Webpage> _mapLinePage;
				boost::shared_ptr<const cms::Webpage> _dateTimePage;
				boost::shared_ptr<const cms::Webpage> _stopCellPage;
				boost::shared_ptr<const cms::Webpage> _serviceCellPage;
				boost::shared_ptr<const cms::Webpage> _junctionPage;
				boost::shared_ptr<const cms::Webpage> _ticketCellPage;
				boost::shared_ptr<const cms::Webpage> _mapStopCellPage;
				boost::shared_ptr<const cms::Webpage> _mapServiceCellPage;
				boost::shared_ptr<const cms::Webpage> _mapJunctionPage;
				boost::shared_ptr<const cms::Webpage> _resultRowPage;
			//@}

			/// @name Result
			//@{
				mutable boost::shared_ptr<PTRoutePlannerResult> _result;
			//@}

		public:
			//! @name Getters
			//@{
				const road::RoadModule::ExtendedFetchPlaceResult& getDeparturePlace() const { return _departure_place; }
				const road::RoadModule::ExtendedFetchPlaceResult& getArrivalPlace() const { return _arrival_place; }
				const boost::posix_time::ptime& getStartDepartureDate() const { return _startDate; }
				const boost::posix_time::ptime& getStartArrivalDate() const { return _startArrivalDate; }
				const boost::posix_time::ptime& getEndDepartureDate() const { return _endDate; }
				const boost::posix_time::ptime& getEndArrivalDate() const { return _endArrivalDate; }
				boost::optional<std::size_t> getMaxSolutionsNumber() const { return _maxSolutionsNumber; }
				boost::optional<boost::posix_time::time_duration> getMaxTransferDuration() const { return _maxTransferDuration; }
				const graph::AccessParameters& getAccessParameters() const { return _accessParameters; }
				boost::shared_ptr<const pt_website::RollingStockFilter> getTransportModeFilter() const { return _rollingStockFilter; }
				const std::string& getOutputFormat() const { return _outputFormat; }
				const algorithm::AlgorithmLogger& getLogger() const { return *_logger; }
				const boost::shared_ptr<PTRoutePlannerResult>& getResult() const { return _result; }
			//@}

			//! @name Setters
			//@{
				void setMaxSolutions(boost::optional<std::size_t> value){ _maxSolutionsNumber = value; }
				void setOutputFormat(const std::string& value){ _outputFormat = value; }
				void setStartArrivalDate(const boost::posix_time::ptime& value){ _startArrivalDate = value; }
				void setEndDepartureDate(const boost::posix_time::ptime& value){ _endDate = value; }
				void setOriginCityText(const std::string& value){ _originCityText = value; }
				void setOriginPlaceText(const std::string& value){ _originPlaceText = value; }
				void setDestinationCityText(const std::string& value){ _destinationCityText = value; }
				void setDestinationPlaceText(const std::string& value){ _destinationPlaceText = value; }
			//@}

			/// @name Modifiers
			//@{
				void setDeparturePlace(const boost::shared_ptr<geography::Place>& value);
				void setArrivalPlace(const boost::shared_ptr<geography::Place>& value);
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_request
			virtual util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_request
			virtual void _setFromParametersMap(const util::ParametersMap& map);



			RoutePlannerFunction();



			/** Action to run, defined by each subclass.
			*/
			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



			virtual bool isAuthorized(const server::Session* session) const;



			virtual std::string getOutputMimeType() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of a result on a webpage.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Main-page
			//////////////////////////////////////////////////////////////////////////
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// @param page page to use for display
			/// @param request current request
			/// @param object result to display
			void display(
				std::ostream& stream,
				const server::Request& request,
				const PTRoutePlannerResult& object,
				const boost::gregorian::date& date,
				size_t periodId,
				const geography::Place* originPlace,
				const geography::Place* destinationPlace,
				const pt_website::HourPeriod* period,
				const graph::AccessParameters& accessParameters
			) const;


		private:
			void _xmlDisplayConnectionPlace(
				std::ostream& stream,
				const geography::NamedPlace& place,
				bool showCoords
			) const;
			void _xmlDisplayPhysicalStop(
				std::ostream& stream,
				const std::string& tag,
				const pt::StopPoint& place,
				bool showCoords
			) const;
			void _xmlDisplayAddress(
				std::ostream& stream,
				const geography::NamedPlace& place,
				bool showCoords
			) const;
			void _xmlDisplayAddress(
				std::ostream& stream,
				const road::Crossing& place,
				const road::RoadPlace& roadPlace,
				bool showCoords
			) const;
			void _xmlDisplayRoadPlace(
				std::ostream& stream,
				const road::RoadPlace& roadPlace,
				bool showCoords
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Display a row on a web page.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Schedules-sheet-row
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to write on
			/// @param request current request
			/// @param place place corresponding to the displayed row
			/// @param cells cells content
			/// @param alternateColor (1) indicates if the line rank is odd or even. Values :
			///		- false/0 = odd
			///		- true/1 = even
			///	@param place (Place* object) : Corresponding place of the line
			///	@param site Displayed site
			void _displayRow(
				std::ostream& stream,
				const server::Request& request,
				const geography::NamedPlace& place,
				const std::string& cells,
				bool alternateColor,
				bool isOrigin,
				bool isDestination
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of schedule sheet cell.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Schedules-sheet-cell
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to write on
			/// @param request current request
			/// @param isItFirstRow (0) Is the cell the first departure or arrival ?
			/// @param isItLastRow (1) Is the cell the last departure or arrival ?
			/// @param columnNumber (2) Rank of the column from left to right
			/// @param isItFootLine (3) Is the cell on a pedestrian junction ?
			/// @param firstTime (4) Start of continuous service, Time else
			/// @param lastTime (5) End of continuous service, Time else
			/// @param isItContinuousService (6) Is the cell on a continuous service ?
			/// @param isFirstWriting (7) Is it the first time that we write on the column ?
			/// @param isLastWriting (8) Is it the last time that we write on the column ?
			void _displayScheduleCell(
				std::ostream& stream,
				const server::Request& request,
				size_t columnNumber,
				bool isItFootLine,
				const boost::posix_time::time_duration& firstTime,
				const boost::posix_time::time_duration& lastTime,
				bool isItContinuousService,
				bool isFirstWriting,
				bool isLastWriting,
				bool isFirstFoot,
				bool isOriginRow,
				bool isDestinationRow
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Output of empty cells before the next place with content.
			void _displayEmptyCells(
				const server::Request& request,
				const PTRoutePlannerResult::PlacesListConfiguration::List& placesList,
				PlacesContentVector::iterator& itSheetRow,
				PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator& itPlaces,
				const geography::NamedPlace& placeToSearch,
				std::size_t columnNumber,
				bool displayFoot
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of schedule sheet lines list cell.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Line-cell
			//////////////////////////////////////////////////////////////////////////
			///	@param stream Stream to write on
			///	@param request current request
			///	@param columnNumber Column rank from left to right
			///	@param journey The journey to describe
			/// @pre _linesRowPage is not null
			void _displayLinesCell(
				std::ostream& stream,
				const server::Request& request,
				std::size_t columnNumber,
				const graph::Journey& journey
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of schedule sheet lines warning cell.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Warning-cell
			//////////////////////////////////////////////////////////////////////////
			///	@param stream Stream to write on
			///	@param request current request
			///	@param columnNumber Column rank from left to right
			///	@param journey The journey to describe
			/// @pre _warningPage is not null
			void _displayWarningCell(
				std::ostream& stream,
				const server::Request& request,
				std::size_t columnNumber,
				const graph::Journey& journey
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of route planning result row.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Result-row
			//////////////////////////////////////////////////////////////////////////
			///	@param stream Stream to write on
			///	@param request current request
			///	@param rowNumber Row rank from top to bottom
			///	@param journey The journey to describe
			/// @param firstArrivalTime first arrival time of journey
			/// @param lastArrivalTime last arrival time of journey (if not continuous same value than firstArrivalTime)
			/// @param firstDepartureTime first departure time of journey
			/// @param lastDepartureTime last departure time of journey (if not continuous same value than firstArrivalTime)
			/// @param isItContinuousService Is the journey contains only continuous services ?
			/// @pre _resultRowPage is not null
			void _displayResultRow(
				std::ostream& stream,
				const server::Request& request,
				std::size_t rowNumber,
				const graph::Journey& journey,
				const boost::posix_time::time_duration& firstArrivalTime,
				const boost::posix_time::time_duration& lastArrivalTime,
				const boost::posix_time::time_duration& firstDepartureTime,
				const boost::posix_time::time_duration& lastDepartureTime,
				bool isItContinuousService
			) const;


			//////////////////////////////////////////////////////////////////////////
			/// Journey display.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Journey
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to write on
			///	@param page page to use to display
			///	@param request current request
			///	@param departurePlace Asked departure place
			///	@param arrivalPlace Asked arrival place
			void _displayJourney(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> stopCellPage,
				boost::shared_ptr<const cms::Webpage> serviceCellPage,
				boost::shared_ptr<const cms::Webpage> junctionPage,
				const server::Request& request,
				std::size_t n,
				const graph::Journey& journey,
				const geography::Place& departurePlace,
				const geography::Place& arrivalPlace,
				boost::logic::tribool handicappedFilter,
				boost::logic::tribool bikeFilter,
				bool isTheLast
			) const;



			/** Display of stop cell.
				See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Journey-stop
				@param stream Stream to write on
				@param isItArrival (0) true if the stop is used as an arrival, false else
				@param alarm (1/2) Alert (1=message, 2=level)
				@param isItTerminus (3) true if the stop is the terminus of the used line, false else
				@param place Place to display
				@param color (5) Odd or even color
				@param time Time
				@param continuousServiceRange Continuous service range (if <= 0 then no continuous service displayed)
				@param site Displayed site
			*/
			void _displayStopCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				bool isItArrival,
				bool isItTerminus,
				const pt::StopPoint* arrivalPhysicalStop,
				const pt::StopPoint* departurePhysicalStop,
				bool color,
				const boost::posix_time::ptime& time,
				boost::posix_time::time_duration continuousServiceRange,
				bool isLastLeg,
				bool isFirstLeg
			) const;



			/** Display of junction cell.
				See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Journey-pedestrian-chunk
				@param stream Stream to display on
				@param page page to use to display
				@param request current request
				@param alarm Alarm to display for the road use
				@param color Odd or even row in the journey board
				@param distance Length of the junction
			*/
			void _displayJunctionCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				bool color,
				double distance,
				const geos::geom::Geometry* geometry,
				const road::Road* road,
				const graph::Vertex& departureVertex,
				const graph::Vertex& arrivalVertex,
				bool isLastLeg,
				bool isFirstLeg,
				bool isFirstFoot
			) const;



			/** Display of service cell.
				See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_planner_CMS_response#Journey-service-use
				@param stream Stream to write on
				@param page page to use to display
				@param request current request
				@param serviceUse The service use to display
				@param continuousServiceRange Continuous service range
				@param handicappedFilterStatus (8)
				@param bikeFilterStatus (10)
				@param alarm (19/20) Alarm
				@param color (21) Odd or even color
			*/
			void _displayServiceCell(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const graph::ServicePointer& serviceUse,
				boost::posix_time::time_duration continuousServiceRange,
				boost::logic::tribool handicappedFilterStatus,
				boost::logic::tribool bikeFilterStatus,
				bool color,
				bool isLastLeg,
				bool isFirstLeg
			) const;
		};
}	}

#endif // SYNTHESE_RoutePlannerFunction_H__
