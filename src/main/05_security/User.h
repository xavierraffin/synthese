
#ifndef SYNTHESE_SECURITY_USER_H
#define SYNTHESE_SECURITY_USER_H

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Utilisateur.
			@ingroup m05
		
			Un utilisateur correspond � un identifiant de connexion rattach� � une personne. 
			Les donn�es suivantes lui sont reli�es :
				- un nom d'utilisateur
				- un mot de passe
				- des coordonn�es (mail, etc)
				- un @ref synthese::security::Profile "profil"
				- des param�tres de personnalisation (demandes favorites...)

		*/
		class User
		{
		private:
			Profile*		_profile;
			std::string		_login;
			std::string		_password;
			std::string		_name;
			std::string		_surname;
			

		public:
		};
	}
}