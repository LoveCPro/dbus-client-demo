static void XXX_info_get()
{
    GDBusConnection *bus;
    GDBusMessage *message;
    GDBusMessage *method_reply_message;
    GError *error = NULL;
    GVariant *arg0;
    const gchar *path;
    GVariant *ifaces_and_properties;
    GVariantIter iter;
    char *request = DBUS_ROUTE_REQUEST_NAME;

    const gchar *property_name;
    GVariant *property_value;

    bus = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);

    message = g_dbus_message_new_method_call (DBUS_XXX_TARGET,
            obj_path, // object to call on
            DBUS_XXX_INTERFACE, // interface to call on
            METHOD_GET_XXX);

    g_dbus_message_set_body (message, g_variant_new ("(s)", request));
    method_reply_message = g_dbus_connection_send_message_with_reply_sync (bus,
            message,
            G_DBUS_SEND_MESSAGE_FLAGS_NONE,
            -1,
            NULL, /* out_serial */
            NULL, /* cancellable */
            &error);


    if (method_reply_message == NULL) {
        goto out;
    }

    if (g_dbus_message_get_message_type (method_reply_message) == G_DBUS_MESSAGE_TYPE_ERROR) {
        g_dbus_message_to_gerror (method_reply_message, &error);
        goto out;
    }

    arg0 = g_variant_get_child_value (g_dbus_message_get_body (method_reply_message), 0);
    g_variant_iter_init (&iter, arg0);


    while (g_variant_iter_next (&iter,
            "{&sv}",
            &path,
            &ifaces_and_properties)) {
        if (!strcmp(path, "Name")) {
            /*do some thing*/
        } 
        /*get all val , do some thing*/

        g_variant_unref (ifaces_and_properties);
    }

    g_variant_unref (arg0);



out:
    g_object_unref (message);
    g_object_unref (method_reply_message);

    return ;

}

