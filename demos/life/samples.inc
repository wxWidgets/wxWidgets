/////////////////////////////////////////////////////////////////////////////
// Name:        samples.inc
// Purpose:     Sample configurations for Life!
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// sample configurations
// --------------------------------------------------------------------------

/* Format:
 *
 * Name,
 * Description,
 * Width, Height,
 * Data,                        ('*' = alive, '.' = dead)
 * Field width, Field height,   (optional, defaults to 20 x 20)
 * Wraparound                   (optional, defaults to TRUE)
 * 
 */

LifeShape g_shapes[] =
{
    LifeShape( _("Glider"),
               _("The glider is the first of a series of life forms, known "
                 "as spaceships or fishes, which can travel along the game "
                 "field retaining their original shape."),
               3, 3,
               ".*."
               "..*"
               "***"),
    LifeShape( _("Heavyweight spaceship"),
               _("The glider is just the smaller of the spaceships; this "
                 "one, known as the heavyweight spaceship or 'big fish', "
                 "is the largest spaceship which can travel alone without "
                 "destroying itself. Larger ones can only travel safely "
                 "if they are supported by smaller spaceships."),
               7, 4,
               ".....*."
               "......*"
               "*.....*"
               ".******"),
    LifeShape( _("Eater"),
               _("An eater is any still life that can repair itself from "
                 "some attacks. This one (bottom right), also known as "
                 "'fishhook', eats gliders and fishes (spaceships) provided "
                 "that they approach in a certain angle."),
               10, 10,
               ".*........"
               "..*......."
               "***......."
               ".........."
               ".........."
               ".........."
               "......**.."
               "......*.*."
               "........*."
               "........**" ),
    LifeShape( _("Dice shaker"),
               _("Oscillators have been extensively explored in Life!. "
                 "The dice shaker turns around each seven tics; thus, it "
                 "is an oscillator with a period of fourteen."),
               7, 6,
               ".**.**."
               ".**.**."
               "..*.*.."
               "*.*.*.*"
               "*.*.*.*"
               "**...**" ),
    LifeShape( _("Hertz oscillator"),
               _("The Hertz oscillator is a good example of a set of life "
                 "patterns known as 'billiard tables'. A billiard table is "
                 "an oscillator which is built inside a stable border. In "
                 "particular, this one has a period of eight."),
               14, 11,
               ".....**......."
               ".....**......."
               ".............."
               "**...****...**"
               "*.*.*....*.*.*"
               "..*.**...*.*.."
               "*.*.*....*.*.*"
               "**...****...**"
               ".............."
               ".......**....."
               ".......**....." ),
    LifeShape( _("Phoenix"),
               _("A phoenix is a pattern whose cells all die in every "
                 "generation, and yet lives forever. For example, this is "
                 "a phoenix with period two."),
               8, 8,
               "....*..."
               "..*.*..."
               "......*."
               "**......"
               "......**"
               ".*......"
               "...*.*.."
               "...*...." ),
    LifeShape( _("R-pentomino"),
               _("The R-pentomino is a methuselah - a life form which "
                 "lives for hundreds of generations without stabilizing "
                 "or dying. In particular, the R-Pentomino requires more "
                 "than one thousand tics to reach a stable (periodic) "
                 "state."),
               3, 3,
               ".**"
               "**."
               ".*.",
               80, 80, FALSE ),
    LifeShape( _("Thunderbird"),
               _("The thunderbird is another popular methuselah, which "
                 "doesn't stabilize until the 243th generation. Note that "
                 "because the initial configuration is symmetrical with "
                 "respect to the vertical axis, all generations must be "
                 "symmetrical as well."),
               3, 5,
               "***"
               "..."
               ".*."
               ".*."
               ".*.",
               60, 60, FALSE ),
    LifeShape( _("Accorn"),
               _("Probably the most popular methuselah, the accorn lives "
                 "for 5206 (!) generations. To see it in action, a very "
                 "large game field is needed."),
               7, 3,
               ".*....."
               "...*..."
               "**..***",
               150, 150, FALSE ),
    LifeShape( _("Galaxy"),
               _("One from my personal collection. It is really beautiful "
                 "to see this configuration expand and shrink periodically "
                 "for hundreds of tics before reaching a stable state."),
               13, 13,
               "...***......."
               "......*......"
               "......*......"
               "......*.....*"
               ".....***....*"
               "....*...*...*"
               ".****...****."
               "*...*...*...."
               "*....***....."
               "*.....*......"
               "......*......"
               "......*......"
               ".......***...",
               80, 80, FALSE )
};
