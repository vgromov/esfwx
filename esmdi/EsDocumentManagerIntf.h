#ifndef _es_document_manager_inf_h_
#define _es_document_manager_inf_h_
//--------------------------------------------------------------------------------

/// EsDocumentIntf abstraction
///
ES_DECL_INTF_BEGIN(2CD7ADAB, 5D6B44D8, 858240E5, D6F96343, EsDocumentIntf)

  /// Document state
  ///

  /// Return true if document is managed (attached to manager)
  ES_DECL_INTF_METHOD(bool, isManaged)() const = 0;

  /// Document emptiness check
  ES_DECL_INTF_METHOD(bool, isEmpty)() const = 0;
  /// New document check (never saved)
  ES_DECL_INTF_METHOD(bool, isNew)() const = 0;
  /// Document have unsaved changes
  ES_DECL_INTF_METHOD(bool, isModified)() const = 0;

  /// Document identification
  ///

  /// Return a complex document type. Document manager framework allow to register documents of complex type.
  /// Comlex document type is comprized of Document object type, say DocT, and subtype, say SubT.
  /// Document of complex types are registered by complex type string. 
  /// When document is created, an instance of DocT is actually created, while its content holds SubT data.
  /// For simple type documents, a DocT string is returned.
  ///
  ES_DECL_INTF_METHOD(EsString, complexTypeGet)() const = 0;

  /// Return complex document's subtype. 
  /// For simple type documents, an empty string is returned
  ///
  ES_DECL_INTF_METHOD(EsString, complexSubTypeGet)() const = 0;

  /// Return true, if document type is complex, false otherwise.
  ES_DECL_INTF_METHOD(bool, isComplexDocumentType)() const = 0;

  /// Document key access - unique key for unambiguous document identification 
  /// in document manager collection of existing document object instances
  ///
  ES_DECL_INTF_METHOD(const EsVariant&, keyGet)() const = 0;

  /// Return a copy of document registration information
  ES_DECL_INTF_METHOD(EsVariant, regInfoGet)() const = 0;

  /// Document properties access
  ///

  /// Return true if document property with 'name' exists, false otherwise
  ES_DECL_INTF_METHOD(bool, valueExists)(const EsString& name) const = 0;

  /// Return all assigned property names
  ES_DECL_INTF_METHOD(EsString::Array, valueNamesGet)() const = 0;

  /// Return named property value
  ES_DECL_INTF_METHOD(const EsVariant&, valueGet)(const EsString& name) const = 0;

  /// Assign named property value. If value with such name does not exist in document - create it
  ES_DECL_INTF_METHOD(void, valueSet)(const EsString& name, const EsVariant& value) = 0;

  /// Remove value from the document
  ES_DECL_INTF_METHOD(void, valueDelete)(const EsString& name) = 0;

  /// Document manager object access
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, managerGet)() const = 0;

  /// Document serialization
  ///
  ES_DECL_INTF_METHOD(void, read)(const EsVariant& params = EsVariant::null()) = 0;
  ES_DECL_INTF_METHOD(void, write)(const EsVariant& target, const EsVariant& params = EsVariant::null()) = 0;
  ES_DECL_INTF_METHOD(void, writeCopy)(const EsVariant& target, const EsVariant& params = EsVariant::null()) = 0;

  ES_DECL_INTF_METHOD(void, sideloadRead)() = 0;
  ES_DECL_INTF_METHOD(void, sideloadWrite)() = 0;

  /// Document content initialization
  ES_DECL_INTF_METHOD(void, emptyContentsInit)(const EsVariant& params = EsVariant::null()) = 0;

  /// Document change notification.
  /// @param [in]             subject String parameter, specifying a subject of what have been changed, 
  ///                                 or and empty string, if there are some generic changes
  /// @param [in, optional]   sender  Notification sender object, or nullptr, if notification have no specific origin
  ///
  ES_DECL_INTF_METHOD(void, documentChangedNotify)(const EsString& subject, void* sender) const = 0;

  /// Document sideload change notification.
  /// parameters have the same meaning as ones in documentChangedNotify service
  ///
  ES_DECL_INTF_METHOD(void, documentSideloadChangedNotify)(const EsString& subject, void* sender) const = 0;

ES_DECL_INTF_END
//---------------------------------------------------------------------------------------

/// Document manager interface
///
ES_DECL_INTF_BEGIN(2CD7ABAA, 5D6B44D8, 858240E5, D6F96343, EsDocumentManagerIntf)

  /// Document type registry
  ///

  /// Register document object type, optionally complex one, with optional parameters
  /// @param  [in]            typeName  Registered document type name. If type name is already registered,
  ///                                   an exception is thrown.
  /// @param  [in, optional]  params    An optional parametric payload of the type name being registered.
  ///                                   Interpretation is implementation-dependent.
  ///
  ES_DECL_INTF_METHOD(void, documentRegister)(const EsString& typeName, const EsVariant& params = EsVariant::null()) = 0;

  /// Check if document type is registered
  /// @param  [in]            typeName  Registered document type name. 
  /// @return                           Return true, if type name is registered,
  ///                                   false otherwise.
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentIsRegistered)(const EsString& typeName) const = 0;

  /// Check if document type is registered as singleton one
  /// @param  [in]            typeName  Registered document type name.
  /// @return                           Return true, if type name is registered, and is marked by singleton attribute,
  ///                                   false otherwise.
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentIsSingleton)(const EsString& typeName) const = 0;

  /// Return default extension registry parameter for document type
  /// @param  [in]            typeName  Registered document type name.
  /// @return                           Return default extension for a document type, or throw an exception if
  ///                                   either document type not registered, or a registry entry for a default extension 
  ///                                   is not found.
  ///
  ES_DECL_INTF_METHOD(EsString, documentDefaultExtGet)(const EsString& typeName) const = 0;

  /// Get all registered document types
  ES_DECL_INTF_METHOD(EsString::Array, registeredDocumentTypesGet)() const = 0;

  /// Get optional document type name registry parameters, if any were specified
  /// @param  [in]            typeName  Registered document type name. If unregistered
  ///                                   type name was specified, throw an exception.
  /// @return                           Return optional registration payload, or an empty variant.
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentRegistrationPayloadGet)(const EsString& typeName) const = 0;

  /// Get count of registered document type information entries
  ///
  /// @return                           Count of entries in document registry
  /// 
  ES_DECL_INTF_METHOD(ulong, registeredDocumentCountGet)() const = 0;

  /// Create new registered document object
  /// @param  [in]            typeName  Registered document type name. If unknown type name is specified,
  ///                                   an exception is thrown.
  /// @return                           Pointer to created document object
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentCreate)(const EsString& typeName) = 0;

  /// Create new registered document object, with specific key value
  /// @param  [in]            typeName  Registered document type name. If unknown type name is specified,
  ///                                   an exception is thrown.
  /// @param  [in]            params    Parameters set to use when creating new document.
  ///                                   It may be a collection, or single value. It former, a first collection member 
  ///                                   is considered a document key. Otherwise, it _is_ a document key to assign 
  ///                                   to the newly created document. If an empty string is specified as key,
  ///                                   an automatic value, document object implementation-dependent is assigned.
  ///                                   If a document with such key already exists, throw an exception.
  /// @return                           Pointer to created document object
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentCreate)(const EsString& typeName, const EsVariant& params) = 0;

  /// Create or open existing registered document object with optional parameters
  /// @param  [in]            typeName  Registered document type name. If unknown type name is specified,
  ///                                   an exception is thrown
  /// @param  [in, optional]  params    An additional optional parameters specifying how to open document,
  ///                                   and what to do with it. Interpretation is implementation-dependent.
  /// @return                           Pointer to the open document object
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentOpen)(const EsString& typeName, const EsVariant& params = EsVariant::null()) = 0;

  /// Currently opened documents access
  /// Get list of open documents, optionally filtered by registered document type name.
  /// @param  [in, optional]	typeName	Registered document type name, optional.
  ///									                  If se to an empty string, list of all open documents
  ///									                  is returned.
  /// @return							              A variant collection, each item of which is a document object.
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentsGet)(const EsString& typeName = EsString::null()) const = 0;

  /// Return count of currently open documents
  /// @return							              A count of items in open couments collection 
  ///
  ES_DECL_INTF_METHOD(ulong, documentsCountGet)() const = 0;

  /// Return keys of all open documents
  ///
  /// @return                           Collection of all open documents keys
  /// 
  ES_DECL_INTF_METHOD(EsVariant, allDocumentKeysGet)() const = 0;

  /// Find an existing document object by its key
  /// @param  [in]            key       Document key by which to locate an existing document.
  /// @return                           Found document object pointer, or nullptr, if 
  ///                                   no document is found.
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentFind)(const EsVariant& key) const = 0;

  /// Find the first existing instance of the document of specific type.
  /// Useful when working with singleton documewnts mainly.
  /// @param [in]             type      Document instance type to look up to
  /// @return                           Found document instance, or nullptr, if no document is found
  ///
  ES_DECL_INTF_METHOD(EsVariant, documentFindFirstByType)(const EsString& type) const = 0;

  /// Find first registered document type by file extension
  /// @param [in]             ext       Document file extension to look up registered type for
  /// @return                           Found (first) document type, or empty string, if not found
  /// 
  ES_DECL_INTF_METHOD(EsString, documentTypeFindByFileExt)(const EsString& ext) const = 0;

  /// Close an open document by its key, optionally throwing an exception if document with this key does not
  /// exist.
  /// @param  [in]            key       Key of the document to close. If no open document with specified key
  ///                                   exists, throw an exception, if doThrow parameter is true. 
  ///                                   Do nothing otherwise.
  ES_DECL_INTF_METHOD(void, documentClose)(const EsVariant& key) = 0;

  /// Close all open documents
  ES_DECL_INTF_METHOD(void, documentsCloseAll)() = 0;

  /// Request to close all open documents. Unlike documentsCloseAll(), 
  /// does not explicitly close documents, but rather send request for closure to 
  /// whoever may respond to it ant take neccessary actions.
  /// 
  ES_DECL_INTF_METHOD(void, documentsCloseAllRequest)() = 0;

ES_DECL_INTF_END
//---------------------------------------------------------------------------------------

#endif // _es_document_manager_inf_h_
