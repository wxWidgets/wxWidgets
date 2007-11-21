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
 * Width,
 * Height,
 * Data ('*' = alive, '.' = dead)
 * 
 */

const LifePattern g_patterns[] =
{
    LifePattern(
               _("Glider"),
               _("The glider is the first of a series of life forms, known as spaceships or fishes, which can travel along the game field retaining their original shape."),
               3, 3,
               ".*."
               "..*"
               "***"),
    LifePattern(
               _("Heavyweight spaceship"),
               _("The glider is just the smaller of the spaceships; this one, known as the heavyweight spaceship or 'big fish', is the largest spaceship which can travel alone without destroying itself. Larger ones can only travel safely if they are supported by smaller spaceships."),
               7, 4,
               ".....*."
               "......*"
               "*.....*"
               ".******"),
    LifePattern(
               _("Eater"),
               _("An eater is any still life that can repair itself from some attacks. This one (bottom right), also known as 'fishhook', eats gliders and fishes (spaceships) provided that they approach in a certain angle."),
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
    LifePattern(
               _("Dice shaker"),
               _("Oscillators have been extensively explored in Life!. The dice shaker turns around each seven tics; thus, it is an oscillator with a period of fourteen."),
               7, 6,
               ".**.**."
               ".**.**."
               "..*.*.."
               "*.*.*.*"
               "*.*.*.*"
               "**...**" ),
    LifePattern(
               _("Hertz oscillator"),
               _("The Hertz oscillator is a good example of a set of life patterns known as 'billiard tables'. A billiard table is an oscillator which is built inside a stable border. In particular, this one has a period of eight."),
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
    LifePattern(
               _("Phoenix"),
               _("A phoenix is a pattern whose cells all die in every generation, and yet lives forever. For example, this is a phoenix with period two."),
               8, 8,
               "....*..."
               "..*.*..."
               "......*."
               "**......"
               "......**"
               ".*......"
               "...*.*.."
               "...*...." ),
    LifePattern(
               _("R-pentomino"),
               _("The R-pentomino is a methuselah - a life form which lives for hundreds of generations without stabilizing or dying. In particular, the R-Pentomino requires more than one thousand tics to reach a stable (periodic) state."),
               3, 3,
               ".**"
               "**."
               ".*." ),
    LifePattern(
               _("Thunderbird"),
               _("The thunderbird is another popular methuselah, which doesn't stabilize until the 243th generation. Note that because the initial configuration is symmetrical with respect to the vertical axis, all generations must be symmetrical as well."),
               3, 5,
               "***"
               "..."
               ".*."
               ".*."
               ".*." ),
    LifePattern(
               _("Accorn"),
               _("Probably the most popular methuselah, the accorn lives for 5206 (!) generations."),
               7, 3,
               ".*....."
               "...*..."
               "**..***" ),
    LifePattern(
               _("Galaxy"),
               _("One from my personal collection. It is really beautiful to see this configuration expand and shrink periodically for hundreds of tics before reaching a stable state."),
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
               ".......***..." ),
    LifePattern(
               _("Glider gun"),
               _("A gun is a stationary pattern that emits spaceships forever. The glider gun shown here was the first known gun, and indeed the first known finite pattern with unbounded growth. It was found by Bill Gosper in November 1970. Many new guns have since been found."),
               36, 9,
               ".........................*.........."
               "......................****.........."
               ".............*.......****..........."
               "............*.*......*..*.........**"
               "...........*...**....****.........**"
               "**.........*...**.....****.........."
               "**.........*...**........*.........."
               "............*.*....................."
               ".............*......................" ),
    LifePattern(
               _("Puffer train"),
               _("A puffer is an object that moves like a spaceship, except that it leaves a trail of debris behind. The puffer train is one of the best-known puffers. Originally found by Bill Gosper, this is a very dirty puffer; the tail does not stabilize until generation 5533. It consists of a B-heptomino (the middle pattern) escorted by two light weight space ships."),
               5, 18,
               "...*."
               "....*"
               "*...*"
               ".****"
               "....."
               "....."
               "....."
               "*...."
               ".**.."
               "..*.."
               "..*.."
               ".*..."
               "....."
               "....."
               "...*."
               "....*"
               "*...*"
               ".****" ),
    LifePattern(
               _("Max"),
               _("Max is the fastest-growing known pattern in Conway's Game of Life (possibly the fastest possible).")
               + wxString(_(" It fills space to a density of 1/2, conjectured to be the maximum density, and does it at a speed"))
               + wxString(_(" of c/2 in each of the 4 directions, which has been proven to be the maximum possible speed.\n\nPopulation"))
               + wxString(_(" growth is:\n[(t+19)^2+463]/4 for t divisible by 4;\n[(t+19)^2+487]/4 for t even, not div. by 4;\n"))
               + wxString(_("[(t+18)^2+639]/4 for t odd.\n\nOriginal construction, top/bottom stretchers by Hartmut Holzwart;"))
               + wxString(_(" Size optimization, left/right stretchers by David Bell; Original idea, middle part, left/right stretcher"))
               + wxString(_(" connection by Al Hensel.\n\nThis spacefiller by David Bell, September 1993.")),
               29, 43,
               ".....*.*....................."
               "....*..*....................."
               "...**........................"
               "..*.........................."
               ".****........................"
               "*....*......................."
               "*..*........................."
               "*..*........................."
               ".*.........***...***........."
               "..****.*..*..*...*..*........"
               "...*...*.....*...*..........."
               "....*........*...*..........."
               "....*.*......*...*..........."
               "............................."
               "...***.....***...***........."
               "...**.......*.....*.........."
               "...***......*******.........."
               "...........*.......*........."
               "....*.*...***********........"
               "...*..*..*............**....."
               "...*.....************...*...."
               "...*...*.............*...*..."
               "....*...************.....*..."
               ".....**............*..*..*..."
               "........***********...*.*...."
               ".........*.......*..........."
               "..........*******......***..."
               "..........*.....*.......**..."
               ".........***...***.....***..."
               "............................."
               "...........*...*......*.*...."
               "...........*...*........*...."
               "...........*...*.....*...*..."
               "........*..*...*..*..*.****.."
               ".........***...***.........*."
               ".........................*..*"
               ".........................*..*"
               ".......................*....*"
               "........................****."
               "..........................*.."
               "........................**..."
               ".....................*..*...."
               ".....................*.*....." )
};
