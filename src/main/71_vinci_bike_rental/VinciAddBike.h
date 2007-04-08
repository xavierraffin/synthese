
#ifndef SYNTHESE_VinciAddBike_H__
#define SYNTHESE_VinciAddBike_H__

#include "30_server/Action.h"

namespace synthese
{
	namespace vinci
	{
		/** Bike creation Action Class.
			@ingroup m71Actions refActions
		*/
		class VinciAddBike : public server::Action
		{
		public:
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_MARKED_NUMBER;

		protected:
			std::string _number;
			std::string _marked_number;

			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_VinciAddBike_H__
 
