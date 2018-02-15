#include "dnai/api/api.h"
#include "dnai/http/service.h"

namespace dnai {
const QString api::client_id = "sINQmt18nib3vVlI4B71NKaQjXGWMYxrNJWuuS6e";
const QString api::client_secret = "pMi9ScKMPv3IgHgCQmKHKX7yxJY5KMd2KXfWKRMa2jk1qyiSz7AJqllnvpFIfstnIDkausSlqgoWJabYIkXnPGiXgaKE9ikPeILVvoWlifaFSngX2QIA3sJFWH0EO9oH";
api::User api::user = {};
const Config api::http_config = {
    "http://localhost:8000",
      {},
      {
          [](Url *url) {
            auto token = getToken();
            if (!token.isEmpty()) {
              url->addHeader("Authorization", "Bearer " + token);
          }
      }
    }
  };

    Observable &api::signin(const QString &login, const QString &password)
    {
        return Service::url("oauth", "token")
                ->headers(
                    Headers{
                        {"Authorization", "Basic " + QString(client_id + ":" + client_secret).toUtf8().toBase64()},
                        {"Content-Type", "application/x-www-form-urlencoded"}
                    })
                ->post(Form{
                           {"grant_type", "password"},
                           {"username", login},
                           {"password", password}
                       })
                .map([](Response response) -> Response {
            dnai::api::user = {
                response.body["access_token"].toString(),
                response.body["refresh_token"].toString(),
                QDateTime::currentDateTime().addSecs(response.body["expires_in"].toInt())
            };
            return response;
        });
    }

    void api::logout()
    {
        user = {};
    }

    QString const &api::getToken()
    {
        return user.token;
    }
}