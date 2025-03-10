/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 *
 * Copyright 2014 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <freerdp/config.h>

#include <winpr/assert.h>
#include <winpr/cast.h>

#include <rdtk/rdtk.h>

#include "shadow.h"

#include "shadow_lobby.h"

BOOL shadow_client_init_lobby(rdpShadowServer* server)
{
	BOOL rc = FALSE;
	int width = 0;
	int height = 0;
	rdtkSurface* surface = NULL;
	RECTANGLE_16 invalidRect;
	rdpShadowSurface* lobby = server->lobby;

	if (!lobby)
		return FALSE;

	rdtkEngine* engine = rdtk_engine_new();
	if (!engine)
		return FALSE;

	EnterCriticalSection(&lobby->lock);
	surface =
	    rdtk_surface_new(engine, lobby->data, WINPR_ASSERTING_INT_CAST(uint16_t, lobby->width),
	                     WINPR_ASSERTING_INT_CAST(uint16_t, lobby->height), lobby->scanline);
	if (!surface)
		goto fail;

	invalidRect.left = 0;
	invalidRect.top = 0;
	WINPR_ASSERT(lobby->width <= UINT16_MAX);
	WINPR_ASSERT(lobby->height <= UINT16_MAX);
	invalidRect.right = (UINT16)lobby->width;
	invalidRect.bottom = (UINT16)lobby->height;
	if (server->shareSubRect)
	{
		/* If we have shared sub rect setting, only fill shared rect */
		rectangles_intersection(&invalidRect, &(server->subRect), &invalidRect);
	}

	width = invalidRect.right - invalidRect.left;
	height = invalidRect.bottom - invalidRect.top;
	WINPR_ASSERT(width <= UINT16_MAX);
	WINPR_ASSERT(width >= 0);
	WINPR_ASSERT(height <= UINT16_MAX);
	WINPR_ASSERT(height >= 0);
	rdtk_surface_fill(surface, invalidRect.left, invalidRect.top, (UINT16)width, (UINT16)height,
	                  0x3BB9FF);

	rdtk_label_draw(surface, invalidRect.left, invalidRect.top, (UINT16)width, (UINT16)height, NULL,
	                "Welcome", 0, 0);
	// rdtk_button_draw(surface, 16, 64, 128, 32, NULL, "button");
	// rdtk_text_field_draw(surface, 16, 128, 128, 32, NULL, "text field");

	rdtk_surface_free(surface);

	region16_union_rect(&(lobby->invalidRegion), &(lobby->invalidRegion), &invalidRect);

	rc = TRUE;
fail:
	LeaveCriticalSection(&lobby->lock);
	rdtk_engine_free(engine);
	return rc;
}
