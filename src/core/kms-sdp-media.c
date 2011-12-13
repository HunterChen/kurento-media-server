#include "kms-sdp-media.h"
#include "kms-enums.h"
#include "kms-sdp-payload.h"

#define KMS_SDP_MEDIA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), KMS_TYPE_SDP_MEDIA, KmsSdpMediaPriv))

#define LOCK(obj) (g_static_mutex_lock(&(KMS_SDP_MEDIA(obj)->priv->mutex)))
#define UNLOCK(obj) (g_static_mutex_unlock(&(KMS_SDP_MEDIA(obj)->priv->mutex)))

struct _KmsSdpMediaPriv {
	GStaticMutex mutex;
	gint port;
	KmsMediaType type;
	KmsSdpMode mode;
	glong bandwidth;
	GValueArray *payloads;
};

enum {
	PROP_0,

	PROP_PORT,
	PROP_TYPE,
	PROP_BANDWIDTH,
	PROP_MODE,
	PROP_PAYLOADS
};

G_DEFINE_TYPE(KmsSdpMedia, kms_sdp_media, G_TYPE_OBJECT)

static void
free_payloads(KmsSdpMedia *self) {
	if (self->priv->payloads != NULL) {
		g_value_array_free(self->priv->payloads);
		self->priv->payloads = NULL;
	}
}

static void
kms_sdp_media_set_property(GObject  *object, guint property_id,
				const GValue *value, GParamSpec *pspec) {
	KmsSdpMedia *self = KMS_SDP_MEDIA(object);

	switch (property_id) {
		case PROP_0:
			/* Do nothing */
			break;
		case PROP_TYPE:
			LOCK(self);
			self->priv->type = g_value_get_enum(value);
			UNLOCK(self);
			break;
		case PROP_PORT:
			LOCK(self);
			self->priv->port = g_value_get_int(value);
			UNLOCK(self);
			break;
		case PROP_BANDWIDTH:
			LOCK(self);
			self->priv->bandwidth = g_value_get_long(value);
			UNLOCK(self);
			break;
		case PROP_MODE:
			LOCK(self);
			self->priv->mode = g_value_get_enum(value);
			UNLOCK(self);
			break;
		case PROP_PAYLOADS: {
			GValueArray *va = g_value_get_boxed(value);
			gint i;

			LOCK(self);
			free_payloads(self);
			if (va != NULL)
				self->priv->payloads = g_value_array_copy(va);

			for (i=0; i < self->priv->payloads->n_values; i++) {
				GValue *v;
				KmsSdpPayload *pay;

				v = g_value_array_get_nth(va, i);
				pay = g_value_get_object(v);
				g_object_set(pay, "media", self, NULL);
			}

			UNLOCK(self);
			break;
		}
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
kms_sdp_media_get_property(GObject *object, guint property_id, GValue *value,
							GParamSpec *pspec) {
	KmsSdpMedia *self = KMS_SDP_MEDIA(object);

	switch (property_id) {
		case PROP_TYPE:
			LOCK(self);
			g_value_set_int(value, self->priv->type);
			UNLOCK(self);
			break;
		case PROP_PORT:
			LOCK(self);
			g_value_set_int(value, self->priv->port);
			UNLOCK(self);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

void
kms_sdp_media_dispose(GObject *object) {
	LOCK(object);
	free_payloads(KMS_SDP_MEDIA(object));
	UNLOCK(object);
	/* Chain up to the parent class */
	G_OBJECT_CLASS (kms_sdp_media_parent_class)->dispose(object);
}

void
kms_sdp_media_finalize(GObject *object) {
	KmsSdpMedia *self = KMS_SDP_MEDIA(object);

	g_static_mutex_free(&(self->priv->mutex));
	/* Chain up to the parent class */
	G_OBJECT_CLASS (kms_sdp_media_parent_class)->finalize(object);
}

static void
kms_sdp_media_class_init(KmsSdpMediaClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GParamSpec *pspec, *payload;

	g_type_class_add_private(klass, sizeof (KmsSdpMediaPriv));

	gobject_class->set_property = kms_sdp_media_set_property;
	gobject_class->get_property = kms_sdp_media_get_property;
	gobject_class->dispose = kms_sdp_media_dispose;
	gobject_class->finalize = kms_sdp_media_finalize;

	pspec = g_param_spec_int("port", "Port",
					"The listening port for media",
					0, G_MAXUSHORT, 0,
					G_PARAM_CONSTRUCT_ONLY |
					G_PARAM_READWRITE);

	g_object_class_install_property(gobject_class, PROP_PORT, pspec);

	pspec = g_param_spec_enum("mode", "Connection mode",
				"The connection mode for this media type",
				KMS_SDP_MODE, KMS_SDP_MODE_INACTIVE,
				G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_property(gobject_class, PROP_MODE, pspec);

	pspec = g_param_spec_enum("type", "Media Type",
				  "The connection media type",
			   KMS_MEDIA_TYPE, KMS_MEDIA_TYPE_UNKNOWN,
			   G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_property(gobject_class, PROP_TYPE, pspec);

	pspec = g_param_spec_long("bandwidth", "Bandwidth",
				"The media type bandwidth",
				0L, G_MAXLONG, 0,
				G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_property(gobject_class, PROP_TYPE, pspec);

	payload = g_param_spec_object("payload", "Payload",
					"A supported media format",
					KMS_TYPE_SDP_PAYLOAD,
					G_PARAM_READWRITE);

	pspec = g_param_spec_value_array("payloads", "Payloads",
					"Supported media format descriptions",
					payload, G_PARAM_CONSTRUCT_ONLY |
					G_PARAM_READWRITE);

	g_object_class_install_property(gobject_class, PROP_PAYLOADS, pspec);
}

static void
kms_sdp_media_init(KmsSdpMedia *self) {
	self->priv = KMS_SDP_MEDIA_GET_PRIVATE (self);

	g_static_mutex_init(&(self->priv->mutex));
	self->priv->type = KMS_MEDIA_TYPE_UNKNOWN;
	self->priv->mode = KMS_SDP_MODE_INACTIVE;
	self->priv->port = 0;
	self->priv->bandwidth = -1;
	self->priv->payloads = NULL;
}