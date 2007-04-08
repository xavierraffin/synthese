
/** VinciBikeRentalModule class implementation.
	@file VinciBikeRentalModule.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Rapha�l Murat - Vinci Park <rmurat@vincipark.com>

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

#include <vector>

#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/Profile.h"
#include "12_security/GlobalRight.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Currency.h"
#include "57_accounting/CurrencyTableSync.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;

namespace synthese
{
	using namespace security;
	using namespace accounts;
	using namespace util;

	namespace vinci
	{
		boost::shared_ptr<Account> VinciBikeRentalModule::_freeLockRent;

		const string VinciBikeRentalModule::CSS_LIMITED_HEIGHT = "limitedheight";

		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE = "4111";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE = "4117";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE = "4119";
		const std::string VinciBikeRentalModule::VINCI_STOCKS_BIKE_ACCOUNT_CODE = "371";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE = "70831";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE = "70832";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE = "70833";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_DELAYED_PAYMENTS_ACCOUNT_CODE = "763";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_UNRETURNED_BIKE_ACCOUNT_CODE = "707";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CHECKS_ACCOUNT_CODE = "5112";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE = "5331";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE = "5332";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CREDIT_CARD_ACCOUNT_CODE = "5121";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CASH_ACCOUNT_CODE = "532";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE = "59";

		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_USER = "Vinci";
		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_PROFILE = "VinciAccounts";

		const std::string VinciBikeRentalModule::VINCI_ADMIN_PROFILE = "VinciAdministrator";
		const std::string VinciBikeRentalModule::VINCI_OPERATOR_PROFILE = "VinciOperator";

		// Currencies
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO_NAME = "Euro";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO = "EUR";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_BIKE_NAME = "velo(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_BIKE = "_bike";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING_NAME = "validation(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING = "_punching";

		// Rights
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_PROFILE = "VinciCustomerWithoutAccess";
		
		void VinciBikeRentalModule::initialize()
		{
			// Profile for bike rental administrator user
			Profile* adminProfile;
			vector<Profile*> profiles = ProfileTableSync::search(VINCI_ADMIN_PROFILE);
			if (profiles.size() == 0)
				adminProfile = new Profile;
			else
				adminProfile = profiles.front();
			adminProfile->setName(VINCI_ADMIN_PROFILE);
			Right* r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::DELETE);
			r->setPublicLevel(Right::DELETE);
			adminProfile->cleanRights();
			adminProfile->addRight(r);
			ProfileTableSync::save(adminProfile);

			// Profile for bike rental operator user
			Profile* operatorProfile;
			profiles = ProfileTableSync::search(VINCI_OPERATOR_PROFILE);
			if (profiles.size() == 0)
				operatorProfile = new Profile;
			else
				operatorProfile = profiles.front();
			operatorProfile->setName(VINCI_OPERATOR_PROFILE);
			r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::USE);
			r->setPublicLevel(Right::USE);
			operatorProfile->cleanRights();
			operatorProfile->addRight(r);
			ProfileTableSync::save(operatorProfile);

			// Profile for virtual owner user
			Profile* vinciProfile;
			profiles = ProfileTableSync::search(VINCI_ACCOUNTING_PROFILE);
			if (profiles.size() == 0)
				vinciProfile = new Profile;
			else
				vinciProfile = profiles.front();
			vinciProfile->setName(VINCI_ACCOUNTING_PROFILE);
			r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::FORBIDDEN);
			r->setPublicLevel(Right::FORBIDDEN);
			vinciProfile->cleanRights();
			vinciProfile->addRight(r);
			ProfileTableSync::save(vinciProfile);
		
			// Virtual owner user
			User* vinciUser;
			vector<User*> users = UserTableSync::search(VINCI_ACCOUNTING_USER, VINCI_ACCOUNTING_USER, vinciProfile->getKey());
			if (users.size() == 0)
			{
				vinciUser = new User;
				vinciUser->setName(VINCI_ACCOUNTING_USER);
				vinciUser->setLogin(VINCI_ACCOUNTING_USER);
				vinciUser->setProfile(vinciProfile);
				UserTableSync::save(vinciUser);
			}
			else
			{
				vinciUser = users.front();
			}

			// Required currencies
			Currency* euroCurrency;
			vector<Currency*> currencies = CurrencyTableSync::search(VINCI_CURRENCY_EURO_NAME, VINCI_CURRENCY_EURO);
			if (currencies.size() == 0)
			{
				euroCurrency = new Currency;
				euroCurrency->setName(VINCI_CURRENCY_EURO_NAME);
				euroCurrency->setSymbol(VINCI_CURRENCY_EURO);
				CurrencyTableSync::save(euroCurrency);
			}
			else
				euroCurrency = currencies.front();

			Currency* bikeCurrency;
			currencies = CurrencyTableSync::search(VINCI_CURRENCY_BIKE_NAME, VINCI_CURRENCY_BIKE);
			if (currencies.size() == 0)
			{
				bikeCurrency = new Currency;
				bikeCurrency->setName(VINCI_CURRENCY_BIKE_NAME);
				bikeCurrency->setSymbol(VINCI_CURRENCY_BIKE);
				CurrencyTableSync::save(bikeCurrency);
			}
			else
				bikeCurrency = currencies.front();

			Currency* ticketCurrency;
			currencies = CurrencyTableSync::search(VINCI_CURRENCY_TICKET_PUNCHING_NAME, VINCI_CURRENCY_TICKET_PUNCHING);
			if (currencies.size() == 0)
			{
				ticketCurrency = new Currency;
				ticketCurrency->setName(VINCI_CURRENCY_TICKET_PUNCHING_NAME);
				ticketCurrency->setSymbol(VINCI_CURRENCY_TICKET_PUNCHING);
				CurrencyTableSync::save(ticketCurrency);
			}
			else
				ticketCurrency = currencies.front();

			// Guarantee accounts
			Account* guaranteeAccount;
			vector<Account*> accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				guaranteeAccount = new Account;
				guaranteeAccount->setRightClassNumber(VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE);
				guaranteeAccount->setRightCurrency(euroCurrency);
				guaranteeAccount->setLeftCurrency(euroCurrency);
				guaranteeAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(guaranteeAccount);
			}

			Account* checkGuaranteeAccount;
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				checkGuaranteeAccount = new Account;
				checkGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE);
				checkGuaranteeAccount->setRightCurrency(euroCurrency);
				checkGuaranteeAccount->setLeftCurrency(euroCurrency);
				checkGuaranteeAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(checkGuaranteeAccount);
			}

			Account* cardGuaranteeAccount;
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				cardGuaranteeAccount = new Account;
				cardGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE);
				cardGuaranteeAccount->setRightCurrency(euroCurrency);
				cardGuaranteeAccount->setLeftCurrency(euroCurrency);
				cardGuaranteeAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(cardGuaranteeAccount);
			}

			// Customer accounts
			Account* customerTicketAccount;
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				customerTicketAccount = new Account;
				customerTicketAccount->setRightClassNumber(VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE);
				customerTicketAccount->setRightCurrency(ticketCurrency);
				customerTicketAccount->setLeftCurrency(ticketCurrency);
				customerTicketAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(customerTicketAccount);
			}

			Account* customerEuroAccount;
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				customerEuroAccount = new Account;
				customerEuroAccount->setRightClassNumber(VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE);
				customerEuroAccount->setRightCurrency(ticketCurrency);
				customerEuroAccount->setLeftCurrency(ticketCurrency);
				customerEuroAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(customerEuroAccount);
			}

			// Services accounts
			Account* ticketBikeRent;
			accounts = AccountTableSync::search(getVinciUser()->getKey(), VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE, 0, "");
			if (accounts.empty())
				ticketBikeRent = new Account;
			else
				ticketBikeRent = accounts.front();
			ticketBikeRent->setRightClassNumber(VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
			ticketBikeRent->setRightCurrency(ticketCurrency);
			ticketBikeRent->setLeftCurrency(ticketCurrency);
			ticketBikeRent->setRightUserId(vinciUser->getKey());
			AccountTableSync::save(ticketBikeRent);
			delete ticketBikeRent;
		
			accounts = AccountTableSync::search(getVinciUser()->getKey(), VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE, 0, "");
			if (accounts.empty())
				_freeLockRent.reset(new Account);
			else
				_freeLockRent.reset(accounts.front());
			_freeLockRent->setRightClassNumber(VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE);
			_freeLockRent->setRightCurrency(euroCurrency);
			_freeLockRent->setLeftCurrency(euroCurrency);
			_freeLockRent->setRightUserId(vinciUser->getKey());
			AccountTableSync::save(_freeLockRent.get());
			

			// Payment accounts
			Account* ticketPunchings;
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				ticketPunchings= new Account;
				ticketPunchings->setRightClassNumber(VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE);
				ticketPunchings->setRightCurrency(ticketCurrency);
				ticketPunchings->setLeftCurrency(ticketCurrency);
				ticketPunchings->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ticketPunchings);
			}

			// Stock accounts
			Account* bikeStockAccount;
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_STOCKS_BIKE_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				bikeStockAccount = new Account;
				bikeStockAccount->setRightClassNumber(VINCI_STOCKS_BIKE_ACCOUNT_CODE);
				bikeStockAccount->setRightCurrency(bikeCurrency);
				bikeStockAccount->setLeftCurrency(bikeCurrency);
				bikeStockAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(bikeStockAccount);
			}


			
			// Special profile for customers
			Profile* vinciCustomerProfile;
			profiles = ProfileTableSync::search(VINCI_CUSTOMER_PROFILE);
			if (profiles.size() == 0)
				vinciCustomerProfile = new Profile;
			else
				vinciCustomerProfile = profiles.front();
			vinciCustomerProfile->setName(VINCI_CUSTOMER_PROFILE);
			r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::FORBIDDEN);
			r->setPublicLevel(Right::FORBIDDEN);
			vinciCustomerProfile->cleanRights();
			vinciCustomerProfile->addRight(r);
			ProfileTableSync::save(vinciCustomerProfile);
		}

		Profile* VinciBikeRentalModule::getCustomerProfile()
		{
			vector<Profile*> profiles = ProfileTableSync::search(VINCI_CUSTOMER_PROFILE);
			if (profiles.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the customer profile is missing");
			return profiles.front();
		}

		User* VinciBikeRentalModule::getVinciUser()
		{
			vector<Profile*> profiles = ProfileTableSync::search(VINCI_ACCOUNTING_PROFILE);
			if (profiles.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the rental root user is missing");
			Profile* vinciProfile = profiles.front();
			vector<User*> users = UserTableSync::search(VINCI_ACCOUNTING_USER, VINCI_ACCOUNTING_USER, vinciProfile->getKey());
			if (users.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the rental root user is missing");
			return users.front();
		}

		Currency* VinciBikeRentalModule::getEuroCurrency()
		{
			vector<Currency*> currencies = CurrencyTableSync::search(VINCI_CURRENCY_EURO_NAME, VINCI_CURRENCY_EURO);
			if (currencies.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the euro currency is missing");
			return currencies.front();
		}

		Account* VinciBikeRentalModule::getAccount(const std::string& code)
		{
			vector<Account*> accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), code, 0, "");
			if (accounts.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the " + code + " account is missing");
			return accounts.front();
		}

		accounts::Account* VinciBikeRentalModule::getFreeLockRentServiceAccount()
		{
			return _freeLockRent.get();
		}
	}
}
