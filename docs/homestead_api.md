# Homestead - API Guide

Homestead provides a RESTful API. This is used by the Sprout component.
All access must go via this API, rather than directly to the database or HSS.

## IMPI

    `/impi/<private ID>/digest`

Make a GET request to this URL to retrieve the digest of the specified private ID

The URL takes an optional query parameter: `public_id=<public_id>` If specified a digest is only returned if the private ID is able to authenticate the public ID.

Response:

* 200 if the digest is found, returned as JSON: `{ "digest_ha1": "<DIGEST>" }`
* 404 if the digest is not found.

    `/impi/<private ID>/registration-status?impu=<impu>[&visited-network=<domain>][&auth-type=<type>]`

Make a GET request to this URL to request authorization of a registration request from the specified user.

The URL takes two optional query parameters. The visited-network parameter is used to specify the network providing SIP services to the SIP user. The auth-type parameter is used to determine the type of registration authorization required. It can take the values REG (REGISTRATION), DEREG (DEREGISTRATION) and CAPAB (REGISTRATION_AND_CAPABILITIES).

Response:

* 200 if the user is authorized, returned as JSON. The response will contain the HSS result code, and either the name of a server capable of handling the user, or a list of capabilities that will allow the interrogating server to pick a serving server for the user. This list of capabilities can be empty.
`{ "result-code": "2001",
   "scscf": "<server-name>" }`
`{ "result-code": "2001",
   "mandatory-capabilities": [1,2,3],
   "optional-capabilities": [4,5,6] }`
`{ "result-code": "2001",
   "mandatory-capabilties": [],
   "optional-capabilities": [] }`
* 403 if the user cannot be authorized.
* 404 if the user cannot be found.
* 500 if the HSS is overloaded.

## IMPU - persistent registration state

This URL controls registration state that persists for the whole duration of a registration - such as the fact that the user is registered, or the XML User-Data retrieved from the HSS. (This is in contrast to registration state which may change from one REGISTER message to the next, such as bindings, which are stored in Sprout's memcached store).

`GET /impu/<public ID>/reg-data` will return the subscriber's current registration state; as a GET request, this is guaranteed not to change any state. The body of the response is in the format:

```
<ClearwaterRegData>
    <RegistrationState>REGISTERED</RegistrationState>
    <IMSSubscription>...</IMSSubscription>
</ClearwaterRegData>
```

RegistrationState may take the values REGISTERED, UNREGISTERED or NOT_REGISTERED (following the IMS terminology, where an unregistered user is one where an S-CSCF is assigned to provide unregistered service and storing User-Data, and a user who is not assigned to an S-CSCF is not registered). The IMSSubscription XML is as defined in 3GPP TS 29.228.

Changes to registration state can be done by:

`PUT /impu/<public ID>/reg-data[?impi=<private ID>]`

The body of this PUT request is a JSON object, which has a mandatory "reqtype" field, specifying the type of SIP request this state change is based on. The response to a successful PUT request is the same as a GET request (200 OK with an XML body describing the current registration state).

The valid values of reqtype are:

* `reg`, i.e, `{"reqtype": "reg"}` - used to indicate that a REGISTER has triggered the request. This will put the subscriber into REGISTERED state. If the subscriber is not registered, a Server-Assignment-Request will be sent to the HSS with Server-Assignment-Type REGISTRATION. If the subscriber is registered and at least `hss_reregistration_time` seconds have passed since the last Server-Assignment-Request for this subscriber, a Server-Assignment-Request will be sent to the HSS with Server-Assignment-Type RE_REGISTRATION.
* `call` - used to indicate that a non-REGISTER initial request has triggered this request. This will put the subscriber into a callable state (either REGISTERED or UNREGISTERED) - that is, if Clearwater is not currently the assigned S-CSCF for this subscriber, a Server-Assignment-Request will be sent with type UNREGISTERED_USER so that Clearwater can provide unregistered service.
* `dereg-user`, `dereg-timeout`, `dereg-admin` - used to indicate that a deregistration (e.g. a REGISTER with `Expires: 0`, expiry of all bindings, or some other failure) has triggered this request. If a HSS is configured, Clearwater will delete any cached data for the subscriber (putting it in NOT_REGISTERED state) and send a Server-Assignment-Request with an appropriate type (USER_DEREGISTRATION, TIMEOUT_DEREGISTRATION or ADMINISTRATIVE_DEREGISTRATION). If a HSS is not configured, Clearwater will set the user to UNREGISTERED state.
* `dereg-auth-failure` - used to indicate that a registration of a new binding has failed. This doesn't change any state on Homestead (as a registered user who fails to register a second binding shouldn't be de-registered, and an unregistered or not-registered user who fails to register is already in the right state) - it simply triggers a Server-Assignment-Request with Server-Assignment-Type AUTHENTICATION_FAILURE.

These Server-Assignment-Types will specify a User-Name based on the `impi` query parameter (if provided), or on the PrivateID element in the cached User-Data. It will be omitted if neither of these are available.

The following error cases are possible:

* User definitively does not exist (either a 5001 error from the HSS, or having no HSS configured and no record of the user). This triggers a 404 Not Found.
* Attempting to deregister a user which is not in REGISTERED state is not permitted. This triggers a 400 Bad Request and an error log.
* If Homestead is overloaded, a 503 Service Unavailable error is returned.
* If the Cassandra database or the HSS return an error or do not respond, a 502 Bad Gateway error is returned.

## IMPU - location or server capabilities

    `/impu/<public ID>/location?[originating=true][&auth-type=CAPAB]`

Make a GET request to this URL to request the name of the server currently serving the specified user.

The URL takes two optional query parameters. The originating parameter can be set to 'true' to specify that we have an origating request. The auth-type parameter can be set to CAPAB (REGISTRATION_AND_CAPABILIIES) to request S-CSCF capabilities information.

Response:

* 200 if the user is authorized, returned as JSON. The response will contain the HSS result code, and either the name of a server capable of handling the user, or a list of capabilities that will allow the interrogating server to pick a serving server for the user. This list of capabilities can be empty.
`{ "result-code": "2001",
   "scscf": "<server-name>" }`
`{ "result-code": "2001",
   "mandatory-capabilities": [1,2,3],
   "optional-capabilities": [4,5,6] }`
`{ "result-code": "2001",
   "mandatory-capabilties": [],
   "optional-capabilities": [] }`
* 404 if the user cannot be found.
* 500 if the HSS is overloaded.
