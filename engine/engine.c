#include "engine.h"
#include "mem.h"

Color *globalPalette = NULL;
unsigned long gameTicks = 0;
unsigned long index = 0;

void eng_setPalette(Color *palette)
{
    int i = 0;

    if (!palette) {
        return;
    }

    for (i = 0; i < 256; i++) {
        hal_vid_setPal(
            (char)i,
            palette[i].r >> 2,
            palette[i].g >> 2,
            palette[i].b >> 2
        );
    }
}

/* ========================================================================= */
/* MAIN LOOP'S 2D RENDERING                                                  */
/* ========================================================================= */

void eng_renderFrame(unsigned long gametick)
{
    unsigned long i = 0;
    unsigned long frameToRender = 0;
    long totalOffsetX = 0;
    long totalOffsetY = 0;
    int transformationIndex = 0;
    int totalAngle = 0;
    bool hflip = false;

    Asset *asset = NULL;
    Action *action = NULL;
    Animation *actionAnimation = NULL;
    Sprite *actorSprite = NULL;
    Shape *shape = NULL;
    Transformation *transformation = NULL;
    RotationTransformation *rot = NULL;

    if (renderQueue == NULL) {
        logger("[eng_renderFrame]: Render queue is NULL");
        return;
    }

    mem_arena_reset(frameArena);

    for (i = 0; i < SP_MAX_RENDER_ASSETS; i++) {
        if (renderQueue[i] == NULL) {
            continue;
        }

        asset = renderQueue[i]->asset;
        if (!asset) {
            logger("[eng_renderFrame]: Render queue %lu asset is NULL", i);
            continue;
        }

        action = asset->currentAction;
        if (action == NULL) {
            logger(
                "[eng_renderFrame]: Render queue %lu asset action is NULL",
                i
            );
            continue;
        }

        /* Project world coordinates to screen coordinates via macro */
        SP_WORLD_TO_SCREEN(
            renderQueue[i]->coordinates->x,
            renderQueue[i]->coordinates->y,
            sp_globalCamera->position->x,
            sp_globalCamera->position->y,
            sp_globalCamera->resolution->x,
            sp_globalCamera->resolution->y,
            totalOffsetX,
            totalOffsetY
        );

        shape = renderQueue[i]->shape;
        if (shape != NULL) {
            if (shape->type == GM_SHAPE_TYPE_BOX) {
                if (sp_isShapeInFrustrum(
                        totalOffsetX,
                        totalOffsetY,
                        shape
                    )) {
                    if (shape->isVisible &&
                        shape->color != GM_MASK_COLOR) {
                        as_drawBox(shape, (int)totalOffsetX, (int)totalOffsetY);
                    }
                }
            }
        }

        actionAnimation = action->animation;
        if (!actionAnimation || actionAnimation->length <= 0) {
            continue;
        }

        frameToRender = gametick % (unsigned long)actionAnimation->length;
        actorSprite = actionAnimation->frames[frameToRender];
        totalAngle = 0;

        if (actorSprite != NULL) {
            if (!sp_isInFrustrum(
                    (int)totalOffsetX,
                    (int)totalOffsetY,
                    actorSprite
                )) {
                continue;
            }

            for (transformationIndex = 0;
                 transformationIndex < GM_ANIMATION_MAX_TRANSFORMATIONS;
                 transformationIndex++) {
                transformation =
                    actionAnimation->
                    transformationList[transformationIndex];
                if (transformation == NULL) {
                    continue;
                }

                /* ROTATION */
                if (transformation->type == TR_ROTATION) {
                    rot = (RotationTransformation *)transformation->data;
                    if (rot != NULL) {
                        rot->current += rot->angle;
                        if (rot->current >= 360) {
                            rot->current %= 360;
                        }
                        if (rot->current < 0) {
                            rot->current = (rot->current % 360) + 360;
                        }
                        totalAngle += rot->current;
                    }
                }

                /* TRANSLATION */
                if (transformation->type == TR_TRANSLATION) {
                    logger(
                        "[eng_renderFrame]: Translation transformation PENDING"
                    );
                }
            }

            hflip = (renderQueue[i]->pointingTo->x <
                     renderQueue[i]->coordinates->x) ? true : false;

            if (totalAngle % 360 != 0) {
                as_drawBitmapTransform(
                    &actorSprite->bmpData,
                    (int)totalOffsetX,
                    (int)totalOffsetY,
                    (int)actorSprite->maskColor,
                    totalAngle,
                    hflip
                );
            } else {
                as_drawBitmap(
                    &actorSprite->bmpData,
                    (int)totalOffsetX,
                    (int)totalOffsetY,
                    (int)actorSprite->maskColor,
                    hflip
                );
            }
        }
    }
}

