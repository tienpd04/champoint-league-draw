#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint8_t group;
    uint8_t country;
} team_t;

enum
{
    ENG,
    NED,
    ESP,
    FRA,
    GER,
    ITA,
    POR,
    AUS
};
enum
{
    GROUP_A,
    GROUP_B,
    GROUP_C,
    GROUP_D,
    GROUP_E,
    GROUP_F,
    GROUP_G,
    GROUP_H
};

static const team_t GROUP_1_INFO[8] = {
    {.country = ENG, .group = GROUP_A}, //MAN_CITY,
    {.country = ENG, .group = GROUP_B}, //LIVERPOOL
    {.country = NED, .group = GROUP_C}, //AJAX
    {.country = ESP, .group = GROUP_D}, //REAL_MADRID
    {.country = GER, .group = GROUP_E}, //BAYERN
    {.country = ENG, .group = GROUP_F}, //MAN_UTD
    {.country = FRA, .group = GROUP_G}, //LILLE
    {.country = ITA, .group = GROUP_H}  //JUVE
};
static const team_t GROUP_2_INFO[8] = {
    {.country = FRA, .group = GROUP_A}, //PSG
    {.country = ESP, .group = GROUP_B}, //ALTETICO
    {.country = POR, .group = GROUP_C}, //SPORTING
    {.country = ITA, .group = GROUP_D}, //INTER
    {.country = POR, .group = GROUP_E}, //BENFICA
    {.country = ESP, .group = GROUP_F}, //VILLARREAL
    {.country = AUS, .group = GROUP_G}, //SALZBURG
    {.country = ENG, .group = GROUP_H}  //CHEASEA
};

static const char *GROUP_1_TEAM_NAME[] = {
    "MANCHESTER CITY(ENG, A)",
    "LIVERPOOL(ENG, B)",
    "AJAX AMSTERDAM(NED, C)",
    "REAL MADRID(ESP, D)",
    "BAYERN MUNICH(GER, E)",
    "MANCHESTER UNITED(ENG, F)",
    "LILLE(FRA, G)",
    "JUVENTUS(ITA, H)",
};

static const char *GROUP_2_TEAM_NAME[] = {
    "PARIS SAINT GERMAIN(FRA, A)",
    "ALTETICO MADRID(ESP, B)",
    "SPORTING LISBON(POR, C)",
    "INTER MILAN(ITA, D)",
    "BENFICA(POR, E)",
    "VILLARREAL(ESP, F)",
    "SALZBURG(AUS, G)",
    "CHEASEA(ENG, H)",
};

typedef union
{
    uint8_t indexes[8];
    uint64_t __bits;
} draw_t;
// draw_t d present the result of draw (group2[i] - group1[d.indexes[i]] , i: 0->7)

int draw_valid(draw_t d)
{
    size_t i;
    for (i = 0; i < 8; ++i)
    {
        team_t team2 = GROUP_2_INFO[i];
        team_t team1 = GROUP_1_INFO[d.indexes[i]];
        if (team1.country == team2.country || team1.group == team2.group)
            return 0;
    }
    return 1;
}

int next_permute(uint8_t *p, size_t n)
{
    uint8_t *left, *right, *last;
    unsigned temp;
    // Example if the current permutation is (3 5 7 6 4 2 1) then the next permutation is (3 6 1 2 4 5 7)
    // Because in dictionary order (3 6 1 2 4 5 7) is smallest permutation greater than (3 5 7 6 4 2 1).
    if (n < 2)
        return 0;
    // From the last find first i such that p[i] < p[i + 1]
    last = p + n - 1;
    left = last - 1;
    do
    {
        if (*left < *(left + 1))
            break;
    } while (left-- >= p);
    // Not found return 0
    if (left < p)
        return 0;

    // Found: 3 5* 7 6 4 2 1 (* is at left)

    // From the last find the smaller number such that bigger than number at *
    temp = *left;
    for (right = last; *right < temp; --right)
        ;
    //Found: 3 5* 7 6* 4 2 1   (5* at the left, 6* at the right)

    //Swap 5* and 6*: 3 5 7 6 4 2 1 -> 3 6 7 5 4 2 1
    *left = *right;
    *right = temp;

    // Rotate form left + 1 to the last:  3 6* 7 5 4 2 1 -> 3 6* 1 2 4 5 7
    while (++left < last)
    {
        temp = *left;
        *left = *last;
        *last-- = temp;
    }

    return 1;
}

size_t get_all_draw_case(draw_t *all_case)
{
    draw_t permut = {.indexes = {0, 1, 2, 3, 4, 5, 6, 7}};
    size_t all_case_count = 0;
    do
    {
        if (draw_valid(permut))
            all_case[all_case_count++] = permut;
    } while (next_permute(permut.indexes, 8));
    return all_case_count;
}

size_t remove_team(uint8_t *group, size_t idx, size_t n)
{
    while (idx + 1 < n)
    {
        group[idx] = group[idx + 1];
        idx++;
    }
    return idx < n ? n - 1 : n;
}

int keyboard_choose(int range)
{
    fflush(stdin);
    int ch = getchar();
    int choice_idx = ch - '1';
    while (ch != '\n')
        ch = getchar();

    if (choice_idx >= range || choice_idx < 0)
        return -1;
    return choice_idx;
}

void draw()
{
#define EIGHT_PERMUTAION_COUNT (8 * 7 * 6 * 5 * 4 * 3 * 2 * 1)

    draw_t *all_case = (draw_t *)malloc(sizeof(draw_t) * EIGHT_PERMUTAION_COUNT);
    size_t all_case_count, update_case_count;
    all_case_count = get_all_draw_case(all_case);

    uint8_t remain_group1[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    uint8_t remain_group2[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    size_t remain_count;

    uint8_t choose_group1[8];
    uint8_t choose_group2[8];

    uint8_t mask[8];
    uint8_t gr1_possible_index[8];
    size_t gr1_possible_count;

    size_t i, j;
    for (i = 0; i < 8; ++i)
    {
        remain_count = 8 - i;
        // Show the remaining teams in group 1
        printf("Group1 teams:");
        for (j = 0; j < remain_count; j++)
            printf("  %s", GROUP_1_TEAM_NAME[remain_group1[j]]);
        printf("\n");

        // Show the remaining teams in group 1
        printf("Group2 teams:");
        for (j = 0; j < remain_count; j++)
            printf("  %s", GROUP_2_TEAM_NAME[remain_group2[j]]);
        printf("\n\n");

        // Shows options to choose a team in group 2
        printf("Group2 options:");
        for (j = 0; j < remain_count; ++j)
            printf("  %lu: %s", (j + 1), GROUP_2_TEAM_NAME[remain_group2[j]]);

    CHOICE_GROUP2_TEAM:
        printf("\nPlease choose:");
        int choi = keyboard_choose(remain_count);
        if (choi < 0)
            goto CHOICE_GROUP2_TEAM;

        unsigned team_gr2 = remain_group2[choi];
        choose_group2[i] = team_gr2;
        remove_team(remain_group2, choi, remain_count);
        printf("\nChosen team: %s\n", GROUP_2_TEAM_NAME[team_gr2]);

        // Find the possible index of teams in group 1 can meet the chosen team in group 2
        memset(mask, 0, 8);
        for (j = 0; j < all_case_count; ++j)
            mask[all_case[j].indexes[team_gr2]] = 1;

        for (j = 0, gr1_possible_count = 0; j < remain_count; ++j)
            if (mask[remain_group1[j]])
                gr1_possible_index[gr1_possible_count++] = j;

        // Show the teams in group 1 can meet the chosen team in group 2
        printf("Possible teams in group 1:");
        for (j = 0; j < gr1_possible_count; ++j)
            printf("  %lu: %s", (j + 1), GROUP_1_TEAM_NAME[remain_group1[gr1_possible_index[j]]]);

    CHOICE_GROUP1_TEAM:
        printf("\nPlease choose: ");
        choi = keyboard_choose(gr1_possible_count);
        if (choi < 0)
            goto CHOICE_GROUP1_TEAM;

        unsigned choi_gr1_idx = gr1_possible_index[choi];
        unsigned team_gr1 = remain_group1[choi_gr1_idx];
        choose_group1[i] = team_gr1;
        remove_team(remain_group1, choi_gr1_idx, remain_count);

        // Show the chosen team
        printf("Chosen team: %s\n", GROUP_1_TEAM_NAME[team_gr1]);

        // Show the current matches
        printf("\nCurrent draw:\n");
        for (j = 0; j <= i; ++j)
            printf("%s - %s\n", GROUP_2_TEAM_NAME[choose_group2[j]], GROUP_1_TEAM_NAME[choose_group1[j]]);
        printf("\n\n");

        // Update all possible cases after selected
        for (j = 0, update_case_count = 0; j < all_case_count; ++j)
            if (all_case[j].indexes[team_gr2] == team_gr1)
                all_case[update_case_count++] = all_case[j];

        all_case_count = update_case_count;
    }
    free(all_case);
}

int main(int argc, char const *argv[])
{
    draw();
    return 0;
}
